#include <Window.h>

namespace TestClient {
    Window::Window() : m_Window(0)
    {

    }

    Window::~Window() 
    {
        Destroy();
    }

    bool Window::Create(Vec2 Resolution, string Title, bool Resizable, int Major, int Minor, bool Fullscreen)
    {
        m_Rasterizer = new OpenGLRasterizer();
        RasterContext* Context = m_Rasterizer->CreateContext();

        if(!m_Rasterizer->SetContext(Context)) return false;
        
        if(!glfwInit())
        {
            glfwTerminate();
            printf("GLFW Could not be initialized!\n");
            return false;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,Major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,Minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE       ,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE            ,Resizable);
        glfwWindowHint(GLFW_RED_BITS             ,Context->m_Format.rbits);
        glfwWindowHint(GLFW_GREEN_BITS           ,Context->m_Format.gbits);
        glfwWindowHint(GLFW_BLUE_BITS            ,Context->m_Format.bbits);
        glfwWindowHint(GLFW_ALPHA_BITS           ,Context->m_Format.abits);
        
        if(Context->m_Format.Channels == ColorFormat::CC_GRAYSCALE) ERROR("GLFW Does not have clear grayscale support\n");

        m_Window = glfwCreateWindow(Resolution.x, Resolution.y, Title.c_str(), Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
        if(!m_Window)
        {
            glfwTerminate();
            printf("Unable to open glfw window.\n");
            return false;
		}

        MakeCurrent();
        glGetError();
        
        m_Rasterizer->InitializeContext();
        Context->SetResolution(Resolution);
        return true;
    }

    void Window::Destroy()
    {
        if(!m_Window)
            return;

        delete m_Rasterizer;
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
        m_Rasterizer->GetContext()->SetResolution(GetWindowSize());
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