#include <UI/UI.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>

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
    
    
    UIElement::UIElement() : m_RefCount(1), m_ID(0), m_CID(-1), m_Parent(0), m_Manager(0), m_Bounds(new UIRect(0,0,0,0)), m_MeshNeedsUpdate(true),
                        m_Render(0), m_Material(0), m_Initialized(false), m_ChildOffset(Vec2(0,0)), m_ScissorEnabled(false), m_OuterBounds(new UIRect(0,0,0,0))
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
        m_Parent->UpdateOuterBounds();
    }
    void UIElement::AddChild(UIElement* E)
    {
        E->Orphan();
        
        m_Children.push_back(E);
        E->m_Parent = this;
        E->m_CID = m_Children.size() - 1;
        E->_Initialize(m_Manager);
        E->SetPosition(E->GetPosition());
        UpdateOuterBounds();
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
        UpdateOuterBounds();
    }
    void UIElement::_Update(Scalar dt)
    {
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Update(dt);
        Update(dt);
    }
    void UIElement::_Initialize(UIManager* Manager)
    {
        if(m_Initialized == true)
            return;
        m_Manager  = Manager;
        m_Render   = m_Manager->GetRenderer()->CreateRenderObject(ROT_MESH);
        m_Render->SetTransform(Translation(Vec3(m_Bounds->GetPosition(), 0)));
        m_Material = m_Manager->GetRenderer()->CreateMaterial();
        m_Material->SetShader(m_Manager->GetDefaultShader());
        OnInitialize();
        m_Initialized = true;
    }
    void UIElement::_PreRender()
    {
        if(m_ScissorEnabled)
        {
            glEnable(GL_SCISSOR_TEST); // TODO: Abstract this

            Vec2 Resolution = m_Manager->GetResolution();
            Vec2 CamTrans = m_Manager->GetCamera()->GetTransform().GetTranslation().xy();
            Vec2 HalfRes = Resolution * 0.5;
            Vec3 Position = Vec3(m_Bounds->GetPosition(), 0);
            Vec2 Size = m_Bounds->GetDimensions();
            Vec3 SSPosition = Vec3(Position.x + HalfRes.x - CamTrans.x, Resolution.y - HalfRes.y - Position.y - Size.y + CamTrans.y, Position.z);
        
            glScissor(SSPosition.x, SSPosition.y, Size.x, Size.y); // TODO: Abstract this
        }
        PreRender();
    }
    void UIElement::_Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        _PreRender();
        Render(PrimType, ObjectsRendered);
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Render(PrimType, ObjectsRendered);
        _PostRender();
    }
    void UIElement::_PostRender()
    {
        if(m_ScissorEnabled)
            glDisable(GL_SCISSOR_TEST); // TODO: Abstract this
        PostRender();
    }
    void UIElement::SetPosition(Vec3 Pos)
    {
        Vec3 Position = Pos;
        if(m_Parent)
        {
            Vec3 ParentPosition = m_Parent->GetPosition();
            Position += Vec3(ParentPosition.xy() + m_Parent->GetChildOffset(), ParentPosition.z);
        }

        if(m_Render)
            m_Render->SetTransform(Translation(Position));

        m_Bounds->SetPosition(Position.xy());
    }
    void UIElement::SetSize(Vec2 Size)
    {
        m_Bounds->SetDimensions(Size);
        UpdateOuterBounds();
        m_MeshNeedsUpdate = true;
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
    void UIElement::UpdateOuterBounds()
    {
        Vec2 OuterMin = Vec2(m_Bounds->GetPosition());
        if(m_Parent)
            OuterMin -= m_Parent->GetChildOffset();
        Vec2 OuterMax = OuterMin + Vec2(m_Bounds->GetDimensions());
        for(i32 i = 0; i < m_Children.size(); i++)
        {
            Vec2 COuterPos  = m_Children[i]->GetOuterBounds()->GetPosition();
            Vec2 COuterSize = m_Children[i]->GetOuterBounds()->GetDimensions();

            if(COuterPos.x < OuterMin.x) OuterMin.x = COuterPos.x;
            if(COuterPos.y < OuterMin.y) OuterMin.y = COuterPos.y;

            if(COuterSize.x+COuterPos.x > OuterMax.x) OuterMin.x = COuterSize.x+COuterPos.x;
            if(COuterSize.y+COuterPos.y > OuterMax.y) OuterMax.y = COuterSize.y+COuterPos.y;
        }
        m_OuterBounds->Set(OuterMin, OuterMax - OuterMin);
        if(m_Parent)
                m_Parent->UpdateOuterBounds();
    }
    void UIElement::SetChildOffset(Vec2 Off)
    {
        Vec2 Diff = m_ChildOffset - Off;
        for(i32 i = 0; i < m_Children.size(); i++)
        {
            UIElement* Child = m_Children[i];
            Vec3 ChildPos = Child->GetPosition();
            Child->SetPosition(ChildPos + Vec3(Diff, 0));
        }
        m_ChildOffset = Off;
    }
};
