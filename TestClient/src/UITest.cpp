#include <UITest.h>

namespace TestClient
{
    UITest::UITest()
    {
    }
    UITest::~UITest()
    {
    }
    
    void UITest::Initialize()
    {
        InitGUI();
		InitRenderGUI();
		SetFPSPrintFrequency(10.0f);
        
        ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Rasterizer->SetClearColor(Vec4(0,0,1.0f,1.0f));
    }
    void UITest::Run()
    {
        Vec2 LastPos;
        
        while(IsRunning())
        {
        }
    }
    void UITest::Shutdown()
    {
    }
};
