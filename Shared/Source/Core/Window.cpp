#include "Window.h"

Window::Window(const WindowInfo& Info)
    : m_Info(Info)
{
    if(!glfwInit())
    {
        DEBUG_ERROR("Failed to initialize window");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Window::Initialize()
{
    m_GLFWWindow = glfwCreateWindow(m_Info.Width, m_Info.Height, m_Info.Title.c_str(), nullptr, nullptr);

    if (!m_GLFWWindow)
    {
        DEBUG_ERROR("Failed to create window");
        glfwTerminate();
    }
}

void Window::ProcessEvents()
{
    glfwPollEvents();
}

void Window::SetTitle(const std::string& Title)
{
    m_Info.Title = Title;
    glfwSetWindowTitle(m_GLFWWindow, m_Info.Title.c_str());
}

bool Window::WantsExit()
{
    return glfwWindowShouldClose(m_GLFWWindow);
}

Window::~Window()
{
    glfwDestroyWindow(m_GLFWWindow);
    glfwTerminate();
}
