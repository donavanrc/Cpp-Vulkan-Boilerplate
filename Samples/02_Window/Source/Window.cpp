#include "Main/Main.h"
#include "Core/Window.h"

class BasicWindowApp : public MainApplication
{
public:
    void Initialize() override;
    void Run() override;
private:
    std::unique_ptr<Window> m_Window;
};

void BasicWindowApp::Initialize()
{
    WindowInfo Info = {
        .Title = "Hello Window",
        .Width = 800,
        .Height = 600,
    };

    m_Window = std::unique_ptr<Window>(new Window(Info));
    m_Window->Initialize();
}

void BasicWindowApp::Run()
{
    while (!m_Window->WantsExit())
    {
        m_Window->ProcessEvents();
    }
}

START_APPLICATION(BasicWindowApp)