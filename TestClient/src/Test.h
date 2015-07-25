#pragma once
#include <Silk.h>
#include <System/TaskManager.h>
#include <Window.h>

namespace TestClient
{
    class Test
    {
        public:
            Test();
            virtual ~Test();
        
            void Init();
        
            void SetFPSPrintFrequency(Scalar Hz) { m_FramePrintInterval = 1.0f / Hz; }
            bool IsRunning();
            Scalar GetDeltaTime() { return m_DeltaTime; }
        
            virtual void Run() = 0;
            virtual void Shutdown() = 0;
            virtual const char* GetTestName() const = 0;
        
            Window         * m_Window;
            Renderer       * m_Renderer;
            Rasterizer     * m_Rasterizer;
            ShaderGenerator* m_ShaderGenerator;
        
            Camera         * m_Camera;
        
        protected:
            virtual void Initialize() = 0;
            TaskManager* m_TaskManager;
            Scalar m_ElapsedTime;
            Scalar m_LastElapsedTime;
            Scalar m_DeltaTime;
            Scalar m_FramePrintTime;
            Scalar m_FramePrintInterval;
            bool m_DoShutdown;
            Ray m_CursorRay;
    };
};