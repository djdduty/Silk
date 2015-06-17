#ifndef TESTCLIENT_WINDOW_H
#define TESTCLIENT_WINDOW_H

#include <Math/Math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

using namespace Silk;
namespace TestClient {
    class Window {
        public:
            Window();
            ~Window();
            void Create(Vec2 Resolution, string Title, bool Resizable = false, int Major = 3, int Minor = 2, bool Fullscreen = false);
            void MakeCurrent();
            bool GetCloseRequested();
            Vec2 GetWindowSize();
            void SwapBuffers();
            void PollEvents();
            void Destroy();

            GLFWwindow* GetWindow() {return m_Window;}
            f64 GetElapsedTime();

        protected:
            GLFWwindow* m_Window;
    };
}

#endif