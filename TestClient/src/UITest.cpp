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
        /*
        InitGUI();
        InitCursor();
        
        Mesh* m = new Mesh();
        f32 v[18] =
        {
            0  ,  0,0,
            100,  0,0,
            0  ,100,0,
            
            0  ,100,0,
            100,  0,0,
            100,100,0,
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
        
        f32 t[12] =
        {
            0,0,
            1,0,
            0,1,
            
            0,1,
            1,0,
            1,1
        };
        
        m->SetVertexBuffer  (6,v);
        m->SetColorBuffer   (6,c);
        m->SetTexCoordBuffer(6,t);
        
        UIElement   * Button = m_UIManager->CreateElement ();
        Material    * Mat    = m_Renderer ->CreateMaterial();
        RenderObject* Obj    = m_Renderer ->CreateRenderObject(ROT_MESH,false);
        
        Mat   ->SetShader   (m_UIManager->GetDefaultTextureShader());
        Mat   ->SetMap      (Material::MT_DIFFUSE,m_Textures[m_CursorTexIndex]);
        Obj   ->SetMesh     (m,Mat);
        Obj   ->SetTransform(Translation(Vec3(50,50,0)));
        Button->SetObject   (Obj);
        Button->SetSize     (100,100);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
        m_Materials .push_back(Mat   );
        m_Meshes    .push_back(Obj   );
        m_UIElements.push_back(Button);
        
        */
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
                m_UIManager->GetCamera()->SetTransform(Translation(Vec3(Res.x * 0.5f,Res.y * 0.5f,0.0f)));
            
                Vec2 Pos = m_UIManager->GetCursorPosition();
                if(m_UIManager->GetButtonDownDuration(BTN_LEFT_MOUSE) > 0.1)
                {
                    if(m_UIElements[1]->GetArea().Contains(LastPos))
                    {
                        
                        m_UIElements[1]->GetObject()->SetTransform(m_UIElements[1]->GetObject()->GetTransform() * Translation(Vec3(Pos.x - LastPos.x,Pos.y - LastPos.y,0.0f)));
                    }
                }
                
                LastPos = Pos;
            }
        }
    }
    void UITest::Shutdown()
    {
    }
};
