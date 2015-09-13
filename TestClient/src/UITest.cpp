#include <UITest.h>
#include <UI/UIText.h>

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
        
        UIElement   * Button = new UIElement();
        m_UIManager->AddElement(Button);
        
        Material    * Mat    = m_Renderer ->CreateMaterial();
        RenderObject* Obj    = m_Renderer ->CreateRenderObject(ROT_MESH);
        
        Mat   ->SetShader   (m_UIManager->GetDefaultTextureShader());
        Mat   ->SetMap      (Material::MT_DIFFUSE,m_Textures[m_CursorTexIndex]);
        Obj   ->SetMesh     (m,Mat);
        Obj   ->SetTransform(Translation(Vec3(1011,58,0)));
        Button->SetObject   (Obj);
        Button->SetSize     (100,100);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
        m_Materials .push_back(Mat   );
        m_Meshes    .push_back(Obj   );
        m_UIElements.push_back(Button);
        
        Byte* fDat = Load("Common/Font.fnt");
        Font* Fnt = new Font();
        Fnt->Load(fDat);
        
        UIText* Text = new UIText();
        Text->SetObject(m_Renderer->CreateRenderObject(ROT_MESH));
        m_UIManager->AddElement(Text);
        Text->SetMaterial(m_Renderer->CreateMaterial());
        Text->GetMaterial()->SetShader(m_UIManager->GetDefaultTextShader());
        Text->GetMaterial()->SetMap(Material::MT_DIFFUSE,LoadTexture("Common/Font.png"));
        Text->GetObject()->SetTransform(Translation(Vec3(20,20,1)));
        
        Text->SetFont(Fnt);
        Text->SetText("Test Test Test Test Test Test Test Test");
        Text->SetScale(0.5f);
        
        m_UIElements.push_back(Text);
        
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
                /* Dynamic text */
                UIText* Text = (UIText*)m_UIElements[2];
                Text->SetScale(0.1f * tsc);
                Font* Fnt = Text->GetFont();
                for(i32 i = 0;i < Text->GetText().length();i++)
                {
                    Scalar f = (m_Window->GetElapsedTime() * 0.2f) - ((f32)i) / ((f32)Text->GetText().length());
                    Scalar s = 1.5f + (sin(f * 4.0f * PI) * 0.90f);
                    Font::Glyph g = Fnt->GetGlyph(Text->GetText()[i]);
                    Text->SetCharacterColor (i,Vec4(ColorFunc(f),1.0f));
                    Text->SetCharacterScale (i,s);
                    Text->SetCharacterOffset(i,Vec2(0,((g.Size.y * s * Text->GetScale()) - (g.Size.y * Text->GetScale())) * -0.5f));
                }
                m_UIManager->GetCamera()->SetTransform(Translation(Vec3(Res.x * 0.5f,Res.y * 0.5f,0.0f)));
            
                /* Movable thing */
                Vec2 Pos = m_InputManager->GetCursorPosition();
                if(m_InputManager->GetButtonDownDuration(BTN_LEFT_MOUSE) > 0.1)
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
