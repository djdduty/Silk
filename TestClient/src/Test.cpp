#include <Test.h>
#include <Window.h>

namespace TestClient
{
    Ray UnProject(const Vec3& win,const Mat4& v,const Mat4& p,const Vec4& viewport)
	{
        Vec4 ndc;
		ndc.x = (((win.x - viewport.x) / viewport.z) * 2.0f) - 1.0f;
		ndc.y = (((win.y - viewport.y) / viewport.w) * 2.0f) - 1.0f;
        ndc.y *= -1.0f;
        ndc.z =  win.z;
        ndc.w =  1.0f;
        Mat4 vo = v;
        vo[0][3] = v[1][3] = v[2][3] = 0.0f;
        Mat4 m = (vo * p).Inverse();
        Vec4 rVal = ndc;
        
        Scalar W = rVal.x * m.x.w +
                   rVal.y * m.y.w +
                   rVal.z * m.z.w +
                   rVal.w * m.w.w;
        Scalar invW = 1.0f / W;
        
        Vec4    Result((rVal.x * m.x.x + rVal.y * m.y.x + rVal.z * m.z.x + m.w.x) * invW,
                       (rVal.x * m.x.y + rVal.y * m.y.y + rVal.z * m.z.y + m.w.y) * invW,
                       (rVal.x * m.x.z + rVal.y * m.y.z + rVal.z * m.z.z + m.w.z) * invW,
                       (rVal.x * m.x.w + rVal.y * m.y.w + rVal.z * m.z.w + m.w.w) * invW);
        
        Ray r;
        r.Point = Vec3(v[0][3],v[1][3],v[2][3]);
        r.Dir   = (Result.xyz()).Normalized();
        return r;
	}
    
    Test::Test()
    {
    }
    Test::~Test()
    {
        m_Renderer->SetActiveCamera(0);
        if(m_Camera         ) delete m_Camera;
        if(m_ShaderGenerator) delete m_ShaderGenerator;
        if(m_Renderer       ) delete m_Renderer       ;
        if(m_Window         ) delete m_Window         ;
    }
    void Test::Init()
    {
        m_Window = new Window();
        if(m_Window->Create(Vec2(800,600),GetTestName(),true))
        {
            m_Rasterizer = m_Window->GetRasterizer();
            ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
            m_Window->PollEvents();

            m_Renderer = new Renderer(m_Rasterizer);
            m_Rasterizer->SetRenderer(m_Renderer);
        
            m_ShaderGenerator = new ShaderGenerator(m_Renderer);
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            
            m_Camera = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
            m_Renderer->SetActiveCamera(m_Camera);
            
            m_ElapsedTime = m_LastElapsedTime = m_Window->GetElapsedTime();
            m_FrameCounter = 0;
            
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
            m_Window          = 0;
        }
        Initialize();
    }
    bool Test::IsRunning()
    {
        /* End previous frame */
        m_Window->SwapBuffers();
        
        /* Start new frame */
        if(!m_Window->GetCloseRequested() && !m_DoShutdown)
        {
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
            
            m_FrameCounter++;
            m_FramePrintTime += m_DeltaTime;
            if(m_FramePrintTime > m_FramePrintInterval)
            {
                //printf("Average FPS: %0.2f\n",f32(m_FrameCounter) / m_FramePrintInterval);
                
                m_FramePrintTime  = 0.0f;
                m_FrameCounter = 0;
            }
            return true;
        }
        return false;
    }
};