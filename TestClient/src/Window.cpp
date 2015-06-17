#include <Window.h>

namespace TestClient {
    Window::Window() : m_Window(0)
    {

    }

    Window::~Window() 
    {

    }

    void Window::Create(Vec2 Resolution, string Title, bool Resizable, int Major, int Minor, bool Fullscreen)
    {
        if(!glfwInit())
        {
            glfwTerminate();
            printf("GLFW Could not be initialized!\n");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        m_Window = glfwCreateWindow(Resolution.x, Resolution.y, Title.c_str(), Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
        if(!m_Window)
        {
            glfwTerminate();
            printf("Unable to open glfw window.\n");
            return;
		}

        MakeCurrent();
        glGetError();
    }

    void Window::Destroy()
    {
        if(!m_Window)
            return;

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool Window::GetCloseRequested()
    {
        if(m_Window)
            return glfwWindowShouldClose(m_Window);
        else
            return true;
    }

    void Window::MakeCurrent()
    {
        if(m_Window)
            glfwMakeContextCurrent(m_Window);
    }

    Vec2 Window::GetWindowSize() 
    {
        if(m_Window) {
            int width, height;
            glfwGetWindowSize(m_Window, &width, &height);
            return Vec2(width, height);
        }

        return Vec2(0,0);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    f64 GetTime() 
    {
        return glfwGetTime() * 1000.0;
    }
}