#include <UI/UI.h>
#include <Renderer/Mesh.h>

namespace Silk
{
    bool UIRect::Intersects(const UIRect& Rect)
    {
        if(Contains(Rect.m_Position + Vec2(Rect.m_Dimensions.x,0.0f               ))) return true;
        if(Contains(Rect.m_Position + Vec2(0.0f               ,Rect.m_Dimensions.y))) return true;
        if(Contains(Rect.m_Position + Vec2(0.0f               ,0.0f               ))) return true;
        if(Contains(Rect.m_Position + Vec2(Rect.m_Dimensions.x,Rect.m_Dimensions.y))) return true;
        return false;
    }
    bool UIRect::Contains  (const UIRect& Rect)
    {
        return Contains(Rect.m_Position) && Contains(Rect.m_Position + Rect.m_Dimensions);
    }
    bool UIRect::Contains  (const Vec2& Point)
    {
        return (Point.x > m_Position.x && Point.x < (m_Position.x + m_Dimensions.x))
        &&     (Point.y > m_Position.y && Point.y < (m_Position.y + m_Dimensions.y));
    }
    
    
    UIElement::UIElement(UIManager* Mgr) : m_ID(Mgr->NewUID()), m_CID(0), m_Parent(0), m_Mesh(0), m_Manager(Mgr)
    {
    }
    UIElement::~UIElement()
    {
    }

    void UIElement::Orphan()
    {
        if(!m_Parent) return;
        if(m_Parent->m_Children.size() - 1 <= m_CID) return;
        
        for(i32 i = m_CID;i < m_Parent->m_Children.size() - 1;i++)
        {
            m_Parent->m_Children[i] = m_Parent->m_Children[i + 1];
            m_Parent->m_Children[i]->m_CID = i;
        }
        m_Parent->m_Children.pop_back();
        m_Parent = 0;
        m_CID    = 0;
    }
    void UIElement::AddChild(UIElement* E)
    {
        E->Orphan();
        
        m_Children.push_back(E);
        E->m_Parent = this;
        E->m_CID = m_Children.size() - 1;
    }
    void UIElement::SetMesh(Mesh *m)
    {
        if(m_Mesh) m_Mesh->Destroy();
        m_Mesh = m;
    }
    void UIElement::Render()
    {
        if(!m_Mesh) return;
        
        
        
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->Render();
    }
};
