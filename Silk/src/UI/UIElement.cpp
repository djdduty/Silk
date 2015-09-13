#include <UI/UI.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>

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
    
    
    UIElement::UIElement() : m_RefCount(1), m_ID(0), m_CID(-1), m_Parent(0), m_Manager(0), m_Bounds(new UIRect(0,0,0,0)), m_NeedsMeshUpdate(false), m_Render(0), m_Material(0)
    {

    }
    UIElement::~UIElement()
    {
        if(m_CID != -1 && m_Parent)
            m_Parent->RemoveChild(this);
        m_Manager->RemoveElement(this);
        m_Manager->GetRenderer()->Destroy(m_Render);
        m_Material->Destroy();
    }
    i32 UIElement::Destroy()
    {
        m_RefCount--;
        if(m_RefCount == 0)
        {
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
        m_Parent->m_NeedsMeshUpdate = true;
    }
    void UIElement::AddChild(UIElement* E)
    {
        E->Orphan();
        
        m_Children.push_back(E);
        E->m_Parent = this;
        E->m_CID = m_Children.size() - 1;
        m_NeedsMeshUpdate = true;
    }
    void UIElement::RemoveChild(UIElement* E)
    {
        if(E->GetParent() != this) return;
        m_Children.erase(m_Children.begin() + E->m_CID);
        for(i32 i = E->m_CID;i < m_Children.size();i++)
        {
            m_Children[i]->m_CID--;
        }
        E->m_CID = -1;
        m_NeedsMeshUpdate = true;
    }
    void UIElement::_Update(Scalar dt)
    {
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Update(dt);
        Update(dt);
    }
    void UIElement::_Initialize(UIManager* Manager)
    {
        m_Manager  = Manager;
        m_Render   = m_Manager->GetRenderer()->CreateRenderObject(ROT_MESH);
        m_Render->SetTransform(Translation(Vec3(m_Bounds->GetPosition(), 0)));
        m_Material = m_Manager->GetRenderer()->CreateMaterial();
        m_Material->SetShader(m_Manager->GetDefaultShader());
        OnInitialize();
    }
    void UIElement::_Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        //TODO Setup Scissor
        Render(PrimType, ObjectsRendered);
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Render(PrimType, ObjectsRendered);
        //TODO End Scissor
    }
    void UIElement::SetPosition(Vec3 Pos)
    {
        if(m_Render)
            m_Render->SetTransform(Translation(Pos));

        m_Bounds->SetPosition(Pos.xy());
    }
    void UIElement::SetSize(Vec2 Size)
    {
        m_Bounds->SetDimensions(Size);
    }
    void UIElement::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        if(!m_Render)
            return;

        Material* Mat = m_Render->GetMaterial();
        Shader* Shader = Mat->GetShader();
        Shader->Enable();
        RenderObject* Obj = m_Render;
        
        if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
        {
            //Pass material uniforms
            Material* Mat = Obj->GetMaterial();
            Shader->UseMaterial(Mat);
            
            //Pass object uniforms
            if(Shader->UsesUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS))
            {
                Obj->UpdateUniforms();
                Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
            }
            
            //To do: Batching
            i32 Count = 0;
            if(Obj->m_Mesh->IsIndexed()) Count = Obj->m_Mesh->GetIndexCount();
            else Count = Obj->m_Mesh->GetVertexCount();
            
            Obj->m_Object->Render(Obj,PrimType,0,Count);
            
            i32 vc = Obj->m_Mesh->GetVertexCount();
            i32 tc = 0;
            if(PrimType == PT_TRIANGLES     ) tc = vc / 3;
            if(PrimType == PT_TRIANGLE_STRIP
            || PrimType == PT_TRIANGLE_FAN  ) tc = vc - 2;
            
            m_Manager->GetRenderer()->m_Stats.VisibleObjects++;
            m_Manager->GetRenderer()->m_Stats.VertexCount   += vc;
            m_Manager->GetRenderer()->m_Stats.TriangleCount += tc;
            m_Manager->GetRenderer()->m_Stats.DrawCalls     ++   ;
            
            ObjectsRendered->push_back(Obj);
        }    
        Shader->Disable();
    }
};
