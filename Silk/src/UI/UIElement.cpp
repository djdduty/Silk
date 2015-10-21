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
                        m_Render(0), m_Material(0), m_Initialized(false), m_ChildOffset(Vec2(0,0)), m_ScissorEnabled(false), m_OuterBounds(new UIRect(0,0,0,0)),
						m_Enabled(true), m_AutoUpdateChildrenEnabled(true)
    {

    }
    UIElement::~UIElement()
    {
        if(m_CID != -1 && m_Parent) m_Parent->RemoveChild(this);
        if(m_ID != -1) m_Manager->RemoveElement(this);
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
		RaiseViewUpdatedFlag();
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
		RaiseViewUpdatedFlag();
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
        E->m_Parent = 0;
        UpdateOuterBounds();
		RaiseViewUpdatedFlag();
    }
    void UIElement::_Update(Scalar dt)
    {
		if(m_AutoUpdateChildrenEnabled)
		{
		    for(i32 i = 0;i < m_Children.size();i++)
            {
                m_Children[i]->_Update(dt);
            }
		}
		else
		{
			for(i32 i = 0;i < m_UpdateList.size();i++)
			{
				m_UpdateList[i]->_Update(dt);
			}
			m_UpdateList.clear();
		}
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
		RaiseViewUpdatedFlag();
    }
    void UIElement::_PreRender()
    {
        if(m_ScissorEnabled)
        {
            glEnable(GL_SCISSOR_TEST); // TODO: Abstract this
            
            Vec2 Resolution = m_Manager->GetResolution();
            Vec2 CamTrans = m_Manager->GetCamera()->GetTransform().GetTranslation().xy();
            Vec2 HalfRes = Resolution * 0.5;
            
            UIRect Region = ComputeClipRegion();
            Vec2 Position = Region.GetPosition();
            Vec2 Size = Region.GetDimensions();
            Vec2 SSPosition = Vec2(Position.x + HalfRes.x - CamTrans.x,
                                   Resolution.y - HalfRes.y - Position.y - Size.y + CamTrans.y);
        
            glScissor(SSPosition.x, SSPosition.y, Size.x, Size.y); // TODO: Abstract this
        }
        PreRender();
    }
    void UIElement::_Render(PRIMITIVE_TYPE PrimType,SilkObjectVector* ObjectsRendered)
    {
        _PreRender();
        
        //Must offset element positions here
        Vec3 Pos = GetPosition();
        SetPosition(GetAbsolutePosition());
        
        Render(PrimType, ObjectsRendered);
        
        //And also return them to their relative positions
        SetPosition(Pos);
        
        for(i32 i = 0;i < m_Children.size();i++)
        {
            if(m_Children[i]->IsEnabled()) m_Children[i]->_Render(PrimType,ObjectsRendered);
        }
        _PostRender();
    }
    void UIElement::_PostRender()
    {
        if(m_ScissorEnabled) glDisable(GL_SCISSOR_TEST); // TODO: Abstract this
        PostRender();
    }
    void UIElement::SetPosition(Vec3 Pos)
    {
        Vec3 Position = Pos;
        
        if(m_Render)
            m_Render->SetTransform(Translation(Position));

        m_Bounds->SetPosition(Position.xy());
		RaiseViewUpdatedFlag();
    }
    void UIElement::SetSize(Vec2 Size)
    {
        m_Bounds->SetDimensions(Size);
        UpdateOuterBounds();
        m_MeshNeedsUpdate = true;
		RaiseViewUpdatedFlag();
    }
    Vec3 UIElement::GetAbsolutePosition() const
    {
        if(m_Parent) return m_Parent->GetAbsolutePosition() + GetPosition();
        else return GetPosition();
    }
    UIRect UIElement::ComputeClipRegion() const
    {
        Vec2 Pos1 = GetAbsolutePosition().xy();
        Vec2 Pos2 = Pos1 + m_Bounds->GetDimensions();
        
        UIElement* Next = m_Parent;
        while(Next)
        {
            Vec2 p1 = Next->GetAbsolutePosition().xy();
            Vec2 p2 = p1 + Next->GetBounds()->GetDimensions();
            
            if(p1.x > Pos1.x) Pos1.x = p1.x;
            if(p1.y > Pos1.y) Pos1.y = p1.y;
            
            if(p2.x < Pos2.x) Pos2.x = p2.x;
            if(p2.y < Pos2.y) Pos2.y = p2.y;
            Next = Next->m_Parent;
        }
        
        return UIRect(Pos1,Max(Pos2 - Pos1,Vec2(0,0)));
    }
    void UIElement::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        if(!m_Render)
            return;

        Material* Mat = m_Material;
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
		RaiseViewUpdatedFlag();
    }
	bool UIElement::WillRender() const
	{
		bool IsVisible = m_Enabled;

		UIElement* Next = m_Parent;
		while(IsVisible) { if(!Next) break; IsVisible = Next->m_Enabled; Next = Next->m_Parent; }

		return IsVisible;
	}
	void UIElement::RaiseViewUpdatedFlag()
	{
		if(!m_Enabled) return;

		if(m_Parent && m_Parent->IsEnabled()) m_Parent->RaiseViewUpdatedFlag();
		else if(!m_Parent && m_Manager) m_Manager->SetViewNeedsUpdate(true);
	}
};
