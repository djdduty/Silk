#pragma once
#include <Silk.h>
#include <Window.h>

namespace TestClient
{
    typedef struct _Ray
    {
        Vec3 Point;
        Vec3 Dir;
    } Ray;

    Ray UnProject(const Vec3& win,const Mat4& v,const Mat4& p,const Vec4& viewport);
    
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
            Scalar m_ElapsedTime;
            Scalar m_LastElapsedTime;
            Scalar m_DeltaTime;
            i32 m_FrameCounter;
            Scalar m_FramePrintTime;
            Scalar m_FramePrintInterval;
            bool m_DoShutdown;
            Ray m_CursorRay;
    };
};