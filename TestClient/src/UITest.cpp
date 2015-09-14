#include <UITest.h>
#include <UI/UIText.h>
#include <UI/UIPanel.h>

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
        
        Byte* fDat = Load("Common/Font.fnt");
        Font* Fnt = new Font();
        Fnt->Load(fDat);
        Fnt->SetFontImage(LoadTexture("Common/Font.png"));
        m_UIManager->SetFont(Fnt);

        UIPanel* PTest = new UIPanel(Vec2(400,300));
        m_UIManager->AddElement(PTest);
        PTest->SetBackgroundColor(Vec4(0,0,0,0.75));
        PTest->SetPosition(Vec3(100,100,0));
        m_UIElements.push_back(PTest);

        UIText* Test = new UIText();
        Test->SetFont(Fnt);
        Test->SetText("Test Test Test");
        Test->SetTextSize(20);
        Test->SetPosition(Vec3(0,0,0));
        PTest->AddChild(Test);
        m_UIElements.push_back(Test);
        

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
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
