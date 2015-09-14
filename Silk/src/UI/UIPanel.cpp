#include <UI/UIPanel.h>
#include <UI/UI.h>

namespace Silk
{
    UIPanel::UIPanel(Vec2 Size) : UIElement()
    {
        SetSize(Size);
        SetChildOffset(Vec2(-10,0));
        EnableScissor(true);
    }
    UIPanel::~UIPanel()
    {

    }
    void UIPanel::OnInitialize()
    {
    }
    void UIPanel::Update(Scalar dt) 
    {
        if(m_MeshNeedsUpdate)
            GenerateMesh();

        UpdateOuterBounds();
        f32 ScrollAvailable = m_Bounds->GetDimensions().x - m_OuterBounds->GetDimensions().x;
    }
    void UIPanel::GenerateMesh()
    {
        if(!m_Render)
            return;

        Vec2 Size = m_Bounds->GetDimensions();
        //TODO Border radius and the like
        Mesh* m = new Mesh();
        f32 v[18] =
        {
            0     ,0     ,0,
            Size.x,0     ,0,
            0     ,Size.y,0,
            
            0     ,Size.y,0,
            Size.x,0     ,0,
            Size.x,Size.y,0,
        };
    
        //TODO: Make background color a uniform
        f32 c[24] =
        {
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
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

        m_Render->SetMesh(m, m_Material);
        m_MeshNeedsUpdate = false;
    }
}