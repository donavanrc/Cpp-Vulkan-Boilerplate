#pragma once
#include "pch.h"

struct WindowInfo
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
};

class Window 
{
public:
    Window(const WindowInfo& Info);
    ~Window();
    void Initialize();
    void ProcessEvents();
    void SetTitle(const std::string& Title);
    bool WantsExit();
    void* GetNativeWindow() const 
    { 
        return glfwGetWin32Window(m_GLFWWindow); 
    }
private:
    WindowInfo m_Info;
    GLFWwindow* m_GLFWWindow;
};
