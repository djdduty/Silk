#include <UI/UI.h>
#include <Renderer/Renderer.h>

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
    
    
    UIElement::UIElement() : m_RefCount(1), m_ID(0), m_CID(0), m_Parent(0), m_Render(0), m_Manager(0)
    {
    }
    UIElement::~UIElement()
    {
    }
    
    i32 UIElement::Destroy()
    {
        m_RefCount--;
        if(m_RefCount == 0)
        {
            m_Manager->RemoveElement(this);
            delete this;
            return 0;
        }
        return m_RefCount;
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
    void UIElement::SetObject(RenderObject *o)
    {
        if(m_Render) m_Manager->m_Renderer->Destroy(m_Render);
        m_Render = o;
    }
    UIRect UIElement::GetArea() const
    {
        UIRect r;
        r.Set(m_Render->GetTransform().GetTranslation().xy(),m_Dimensions);
        return r;
    }
    void UIElement::_Update(Scalar dt)
    {
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Update(dt);
        Update(dt);
    }
};
