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
    
    
    UIElement::UIElement() : m_RefCount(1), m_ID(0), m_CID(0), m_Parent(0), m_Content(std::vector<UIRenderContent*>()), m_Manager(0), m_MeshNeedsUpdate(true), 
                             m_CurrentState(UIS_DEFAULT), m_TransformNeedsUpdate(true), m_MaterialNeedsUpdate(true),
                             m_ContentNeedsUpdate(true)
    {
        m_States[UIS_DEFAULT] = new UIElementStyle(this);
    }
    UIElement::~UIElement()
    {
        for(i32 i = 0; i < UIS_COUNT; i++)
        {
            if(m_States[i])
                delete m_States[i];
        }
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
    void UIElement::_Update(Scalar dt)
    {
        if(m_ContentNeedsUpdate)
            UpdateContent();
        if(m_MeshNeedsUpdate)
            UpdateMeshes();
        if(m_MaterialNeedsUpdate)
            UpdateMaterials();
        if(m_TransformNeedsUpdate)
            UpdateTransforms();
        
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Update(dt);
        Update(dt);
    }
    void UIElement::_Initialize()
    {}
    void UIElement::UpdateContent()
    {
        OnUpdateContent();
        m_ContentNeedsUpdate = false;
    }
    void UIElement::UpdateTransforms()
    {
        OnUpdateTransforms();
        for(i32 i = 0; i < m_Content.size(); i++) m_Content[i]->UpdateTransform();
        m_TransformNeedsUpdate = false;
    }
    void UIElement::UpdateMaterials()
    {
        OnUpdateMaterials();
        for(i32 i = 0; i < m_Content.size(); i++) m_Content[i]->UpdateMaterial();
        m_MaterialNeedsUpdate = false;
    }
    void UIElement::UpdateMeshes()
    {
        OnUpdateMeshes();
        for(i32 i = 0; i < m_Content.size(); i++) m_Content[i]->UpdateMesh();
        m_MeshNeedsUpdate = false;
    }
    void UIElement::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        for(i32 i = 0; i < m_Content.size(); i++) m_Content[i]->Render(PrimType, ObjectsRendered);
    }
    void UIElement::_Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        Render(PrimType, ObjectsRendered);
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Render(PrimType, ObjectsRendered);
    }
    void UIElement::AddContent(UIRenderContent* Content)
    {
        m_Content.push_back(Content);
    }
    void UIElement::RemoveContent(UIRenderContent* Content)
    {
        for(i32 i = 0; i < m_Content.size(); i++)
            if(m_Content[i] == Content) {
                m_Content.erase(m_Content.begin() + i);
                break;
            }
    }


    void UIElementStyle::BroadcastMeshChange()
    {
        if(m_Parent)
            m_Parent->m_MeshNeedsUpdate = true;
    }

    void UIElementStyle::BroadcastTransformChange()
    {
        if(m_Parent)
            m_Parent->m_TransformNeedsUpdate = true;
    }

    void UIElementStyle::BroadcastMaterialChange()
    {
        if(m_Parent)
            m_Parent->m_MaterialNeedsUpdate = true;
    }

    void UIElementStyle::BroadcastContentChange()
    {
        if(m_Parent)
            m_Parent->m_ContentNeedsUpdate = true;
    }
};
