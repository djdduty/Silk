#include <Test.h>
#include <Window.h>

namespace TestClient
{
    Test::Test()
    {
    }
    Test::~Test()
    {
        m_Renderer->GetScene()->SetActiveCamera(0);
        if(m_Camera         ) delete m_Camera         ;
        if(m_ShaderGenerator) delete m_ShaderGenerator;
        if(m_Renderer       ) delete m_Renderer       ;
        if(m_Window         ) delete m_Window         ;
        if(m_TaskManager    ) delete m_TaskManager    ;
    }
    void Test::Init()
    {
        m_Window = new Window();
        #ifdef __APPLE__
        if(m_Window->Create(Vec2(400,300),GetTestName(),true))
        #else
        if(m_Window->Create(Vec2(800,600),GetTestName(),true))
        #endif
        {
            m_Rasterizer = m_Window->GetRasterizer();
            ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
            m_Window->PollEvents();

            m_TaskManager = new TaskManager();
            m_Renderer = new Renderer(m_Rasterizer,m_TaskManager);
            m_Rasterizer->SetRenderer(m_Renderer);
        
            m_ShaderGenerator = new ShaderGenerator(m_Renderer);
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            
            m_Camera = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
            m_Renderer->GetScene()->SetActiveCamera(m_Camera);
            m_Renderer->GetScene()->SetCullingAlgorithm(new BruteForceCullingAlgorithm(m_Renderer->GetScene(),m_TaskManager));
            
            m_ElapsedTime = m_LastElapsedTime = m_Window->GetElapsedTime();
            
            m_DeltaTime = 1.0f / 60.0f;
            m_FramePrintTime = 0.0f;
            m_FramePrintInterval = 1.5f;
            
            m_Rasterizer->SetClearColor(Vec4(0.1f,0.1f,0.1f,1.0f));
            
            m_Window->PollEvents();
            m_Rasterizer->ClearActiveFramebuffer();
            
            m_DoShutdown = false;
        }
        else
        {
            m_DoShutdown      = true;
            m_Rasterizer      = 0;
            m_Renderer        = 0;
            m_ShaderGenerator = 0;
            delete m_Window;
            delete m_TaskManager;
            m_Window          = 0;
        }
        Initialize();
    }
    bool Test::IsRunning()
    {
        if(m_Renderer->GetRenderStatistics().FrameID > 0)
        {
            /* End previous frame */
            m_Window->SwapBuffers();
        }
        
        /* Start new frame */
        if(!m_Window->GetCloseRequested() && !m_DoShutdown)
        {
            m_TaskManager->EndFrame  ();
            m_TaskManager->BeginFrame();
            
            m_Window->PollEvents();
            m_Rasterizer->ClearActiveFramebuffer();
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            m_Camera->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
            
            f64 x,y;
            glfwGetCursorPos(m_Window->GetWindow(),&x,&y);
            
            GLint Viewport[4];
            glGetIntegerv(GL_VIEWPORT,Viewport);
            
            Mat4 v = m_Camera->GetTransform();
            Mat4 p = m_Camera->GetProjection();
            
            m_CursorRay = UnProject(Vec3(x,y,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
            
            m_LastElapsedTime = m_ElapsedTime;
            m_ElapsedTime     = m_Window->GetElapsedTime();
            m_DeltaTime       = m_ElapsedTime - m_LastElapsedTime;
            
            m_FramePrintTime += m_DeltaTime;
            if(m_FramePrintTime > m_FramePrintInterval)
            {
                const Renderer::RenderStats& Stats = m_Renderer->GetRenderStatistics();
                printf("Average FPS: %0.2f\n"           ,Stats.AverageFramerate.GetAverage());
                printf("Draw calls: %d (Avg: %0.2f)\n",Stats.DrawCalls,Stats.AverageDrawCalls.GetAverage());
                printf("Object Count: %d (Avg: %0.2f)\n",Stats.VisibleObjects,Stats.AverageVisibleObjects.GetAverage());
                
                if(Stats.MultithreadedCullingEfficiency != 0)
                {
                    printf("Cull efficiency: %0.3f (Avg: %0.3f)\n",Stats.MultithreadedCullingEfficiency,Stats.AverageMultithreadedCullingEfficiency.GetAverage());
                }
                
                m_FramePrintTime  = 0.0f;
            }
            return true;
        }
        return false;
    }
};