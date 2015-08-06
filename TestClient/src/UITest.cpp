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
    }
    void UITest::Run()
    {
        while(IsRunning())
        {
            m_Renderer->Render(PT_TRIANGLES);
        }
    }
    void UITest::Shutdown()
    {
    }
};
