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
        Mesh* m = new Mesh();
        f32 v[18] =
        {
            0  ,  0,0,
            200,  0,0,
            0  ,100,0,
            
            0  ,100,0,
            200,  0,0,
            200,100,0,
        };
    
        f32 c[24] =
        {
            BUTTON_COLOR,
            BUTTON_COLOR,
            BUTTON_COLOR,
            
            BUTTON_COLOR,
            BUTTON_COLOR,
            BUTTON_COLOR,
        };
        m->SetVertexBuffer(6,v);
        m->SetColorBuffer (6,c);
        
        m_UIManager->Initialize();
        UIElement   * Button = m_UIManager->CreateElement();
        RenderObject* Obj    = m_Renderer ->CreateRenderObject(ROT_MESH,false);
        Material    * Mat    = m_Renderer ->CreateMaterial();
        
        Mat   ->SetShader(m_UIManager->GetDefaultShader());
        Obj   ->SetMesh  (m,Mat);
        Button->SetObject(Obj);
        Button->SetSize(200,100);
    }
    void UITest::Run()
    {
        while(IsRunning())
        {
            Vec2 Res = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
            m_UIManager->GetCamera()->SetTransform(Translation(Vec3(Res.x * 0.5f,Res.y * 0.5f,0.0f)));
            m_Renderer->Render(PT_TRIANGLES);
        }
    }
    void UITest::Shutdown()
    {
    }
};
