#include <UITest.h>
#include <UI/UIText.h>

#include <UIElements/SliderControl.h>

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
        
        UIElement* Slider = new SliderControl(0, 100);
        Slider->GetCurrentStyle()->SetPosition(Vec3(300,20,0));
        m_UIManager->AddElement(Slider);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        m_UIElements.push_back(Slider);
        
        Byte* fDat = Load("Common/Font.fnt");
        Font* Fnt = new Font();
        Fnt->Load(fDat);

        
        ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_Rasterizer->SetClearColor(Vec4(0,0,0,1.0f));
    }
    void UITest::Run()
    {
        Vec2 LastPos;
        
        while(IsRunning())
        {
            Vec2 Res = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
            
            if(m_UIManager)
            {
                Scalar tsc = Res.y / GetPreferredInitResolution().y;
                m_UIManager->GetCamera()->SetTransform(Translation(Vec3(Res.x * 0.5f,Res.y * 0.5f,0.0f)));
            }
        }
    }
    void UITest::Shutdown()
    {
    }
};
