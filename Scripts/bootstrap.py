import json
import os
import subprocess
import sys
import urllib.request
import zipfile
import ssl
from pathlib import Path
from urllib.parse import parse_qs, urlparse

ssl._create_default_https_context = ssl._create_unverified_context

SCRIPT_DIR = Path(__file__).parent
ROOT_DIR = SCRIPT_DIR.parent
EXTERNAL_DIR = ROOT_DIR / 'External' / 'Source'
ASSETS_DIR = ROOT_DIR / 'Assets'
ARCHIVES_DIR = ASSETS_DIR / 'Archives'
ASSETS_SOURCE_DIR = ASSETS_DIR / 'Source'
DEPENDENCIES_FILE = SCRIPT_DIR / 'dependencies.json'
DEPENDENCIES_STATUS_FILE = SCRIPT_DIR / 'dependencies_status.json'

def load_dependencies(deps_file):
    with open(deps_file, 'r') as f:
        return json.load(f)['dependencies']

def load_tracked_versions(track_file):
    if not os.path.exists(track_file):
        return {}
    with open(track_file, 'r') as f:
        return json.load(f)

def save_tracked_versions(versions, track_file):
    formatted_versions = {}
    for name, info in versions.items():
        formatted_versions[name] = {
            'type': info.get('type', 'unknown'),
            'url': info.get('url', ''),
            'version': info.get('version', info.get('revision', ''))
        }
    with open(track_file, 'w') as f:
        json.dump(formatted_versions, f, indent=4)

def version_is_newer(current, installed):
    if not installed:
        return True
    
    current_parts = current.replace('v', '').split('.')
    installed_parts = installed.replace('v', '').split('.')
    
    for c, i in zip(current_parts, installed_parts):
        c_num = int(''.join(filter(str.isdigit, c)))
        i_num = int(''.join(filter(str.isdigit, i)))
        if c_num > i_num:
            return True
        elif c_num < i_num:
            return False
    return False

def clone_or_update_repo(dep, external_dir):
    name = dep['name']
    url = dep['url']
    revision = dep['revision']
    repo_path = os.path.join(external_dir, name)
    
    if not os.path.exists(repo_path):
        print(f"[BOOTSTRAP] Cloning {name}...")
        subprocess.run(['git', 'clone', url, repo_path], check=True)
    
    print(f"[BOOTSTRAP] Checking out {name} revision {revision}...")
    subprocess.run(['git', 'fetch', '--tags', 'origin'], cwd=repo_path, check=True)
    
    try:
        subprocess.run(['git', 'checkout', revision], cwd=repo_path, check=True)
    except subprocess.CalledProcessError:
        try:
            v_revision = f"v{revision}"
            subprocess.run(['git', 'checkout', v_revision], cwd=repo_path, check=True)
        except subprocess.CalledProcessError:
            raise RuntimeError(f"[BOOTSTRAP] Could not find revision {revision} or {v_revision}")
    
    result = subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=repo_path, capture_output=True, text=True, check=True)
    return result.stdout.strip()

def format_size(size):
    for unit in ['B', 'KB', 'MB', 'GB']:
        if size < 1024:
            return f"{size:.1f}{unit}"
        size /= 1024
    return f"{size:.1f}TB"

def download_file(url, target_path, user_agent=None):
    headers = {
        'User-Agent': user_agent or 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36'
    }
    
    try:
        os.makedirs(os.path.dirname(target_path), exist_ok=True)
        request = urllib.request.Request(url, headers=headers)
        
        with urllib.request.urlopen(request) as response:
            final_url = response.geturl()
            if final_url != url:
                target_path = Path(target_path).parent / os.path.basename(final_url)
                os.makedirs(os.path.dirname(target_path), exist_ok=True)
            
            total_size = int(response.headers.get('Content-Length', 0))
            block_size = 8192
            downloaded_size = 0
            last_percent = -1 
            
            with open(target_path, 'wb') as out_file:
                while True:
                    chunk = response.read(block_size)
                    if not chunk:
                        break
                    
                    out_file.write(chunk)
                    downloaded_size += len(chunk)
                    
                    if total_size > 0:
                        percent = int((downloaded_size / total_size) * 100)
                        if percent != last_percent:
                            formatted_downloaded = format_size(downloaded_size)
                            formatted_total = format_size(total_size)
                            
                            bar_width = 20
                            filled = int(bar_width * downloaded_size / total_size)
                            bar = "█" * filled + "░" * (bar_width - filled)
                            
                            print("\033[K", end='')
                            status = f"[BOOTSTRAP] {bar} {percent}% ({formatted_downloaded}/{formatted_total})"
                            print(f"\r{status}", end='', flush=True)
                            last_percent = percent
                
                print("\033[K", end='')
                print(f"\r[BOOTSTRAP] Download complete: {format_size(downloaded_size)}")
        
        return target_path
    except urllib.error.HTTPError as e:
        print(f"[BOOTSTRAP] Warning: Failed to download {url}: {e}")
        return None
    except Exception as e:
        print(f"[BOOTSTRAP] Warning: Error downloading {url}: {e}")
        return None

def process_archive(dep, archives_dir, source_dir, tracked_versions):
    name = dep['name']
    url = dep['url']
    user_agent = dep.get('user-agent')
    target_source_dir = source_dir / name
    os.makedirs(target_source_dir, exist_ok=True)
    
    print(f"[BOOTSTRAP] Downloading {name}...")
    
    filename = get_filename_from_url(url)
    is_zip = filename.endswith('.zip') or not os.path.splitext(filename)[1]
    archive_path = archives_dir / filename if is_zip else target_source_dir / filename
    
    downloaded_path = download_file(url, archive_path, user_agent)
    if not downloaded_path:
        return False
    
    if is_zip and downloaded_path.suffix == '.zip':
        try:
            with zipfile.ZipFile(downloaded_path, 'r') as zip_ref:
                zip_ref.extractall(target_source_dir)
        except Exception as e:
            print(f"[BOOTSTRAP] Error extracting {name}: {e}")
            return False
    
    tracked_versions[name] = {
        'type': 'archive',
        'url': url,
        'version': os.path.getsize(downloaded_path)
    }
    
    return True

def get_filename_from_url(url):
    if '?' in url:
        parsed = urlparse(url)
        query_params = parse_qs(parsed.query)
        return query_params['file'][0] if 'file' in query_params else os.path.basename(parsed.path)
    return os.path.basename(url)

def should_download_archive(name, url, tracked_versions):
    if name not in tracked_versions:
        return True
    
    tracked = tracked_versions[name]
    if tracked['url'] != url:
        return True
    
    filename = get_filename_from_url(url)
    is_zip = filename.endswith('.zip') or not os.path.splitext(filename)[1]
    
    archive_path = (ARCHIVES_DIR / name).with_suffix('.zip') if is_zip else ASSETS_SOURCE_DIR / name / filename
    if not archive_path.exists():
        archive_path = ARCHIVES_DIR / filename if is_zip else ASSETS_SOURCE_DIR / name / filename
        if not archive_path.exists():
            return True
    
    current_size = os.path.getsize(archive_path)
    if str(current_size) != str(tracked['version']):
        return True
    
    if is_zip:
        target_dir = ASSETS_SOURCE_DIR / name
        if not target_dir.exists() or not any(target_dir.iterdir()):
            return True
    
    return False

def main():
    print("[BOOTSTRAP] Starting...")
    
    os.makedirs(EXTERNAL_DIR, exist_ok=True)
    os.makedirs(ASSETS_SOURCE_DIR, exist_ok=True)
    os.makedirs(ARCHIVES_DIR, exist_ok=True)
    
    dependencies = load_dependencies(DEPENDENCIES_FILE)
    tracked_versions = load_tracked_versions(DEPENDENCIES_STATUS_FILE)
    success_count = 0
    error_count = 0
    
    for dep in dependencies:
        name = dep['name']
        dep_type = dep['type']
        
        try:
            if dep_type == 'git':
                current_version = dep['revision']
                installed_info = tracked_versions.get(name, {})
                installed_version = installed_info.get('version', '')
                
                if version_is_newer(current_version, installed_version):
                    commit_hash = clone_or_update_repo(dep, EXTERNAL_DIR)
                    tracked_versions[name] = {
                        'type': 'git',
                        'url': dep['url'],
                        'version': commit_hash
                    }
                    save_tracked_versions(tracked_versions, DEPENDENCIES_STATUS_FILE)
                    print(f"[BOOTSTRAP] Successfully installed/updated {name} to version {commit_hash}")
                    success_count += 1
                else:
                    print(f"[BOOTSTRAP] Skipping {name}: version {installed_version} already installed")
                    success_count += 1
            
            elif dep_type == 'archive':
                if should_download_archive(name, dep['url'], tracked_versions):
                    if process_archive(dep, ARCHIVES_DIR, ASSETS_SOURCE_DIR, tracked_versions):
                        success_count += 1
                        save_tracked_versions(tracked_versions, DEPENDENCIES_STATUS_FILE)
                    else:
                        error_count += 1
                else:
                    print(f"[BOOTSTRAP] Skipping {name}: already downloaded")
                    success_count += 1
        
        except Exception as e:
            print(f"[BOOTSTRAP] Error processing {name}: {e}", file=sys.stderr)
            error_count += 1
    
    save_tracked_versions(tracked_versions, DEPENDENCIES_STATUS_FILE)
    
    print(f"\n[BOOTSTRAP] Summary:")
    print(f"  Successful: {success_count}")
    if error_count > 0:
        print(f"  Failed: {error_count}")
        sys.exit(1)
    
    print("[BOOTSTRAP] Done!")

if __name__ == '__main__':
    main()