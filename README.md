# C++ Vulkan Boilerplate

A basic Vulkan application template written in C++.

## Prerequisites

| Software | Platform |
| --- | --- |
| CMake 3.26+ | Windows / Linux |
| Python 3.11+ | Windows / Linux |
| GCC 11+ | Windows / Linux |
| Vulkan SDK 1.4.310+ | Windows / Linux |

## Dependencies installation

```bash
Scripts\bootstrap.py
```

## Project building

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

## Project running

```bash
cmake --build build
```