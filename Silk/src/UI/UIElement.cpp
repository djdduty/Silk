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
    
    
    UIElement::UIElement() : m_RefCount(1), m_ID(0), m_CID(0), m_Parent(0), m_Background(0), m_Manager(0), m_MeshNeedsUpdate(true), 
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
        if(m_Background)
            delete m_Background;
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
    {
        m_Background = new UIRenderRectangle(m_Manager);
    }
    void UIElement::UpdateContent()
    {
        m_ContentNeedsUpdate = false;
    }
    void UIElement::UpdateTransforms()
    {
        if(m_Background)
            m_Background->UpdateTransform(GetCurrentStyle());
        m_TransformNeedsUpdate = false;
    }
    void UIElement::UpdateMaterials()
    {
        if(m_Background)
            m_Background->UpdateMaterial(GetCurrentStyle());
        m_MaterialNeedsUpdate = false;
    }
    void UIElement::UpdateMeshes()
    {
        if(m_Background)
            m_Background->UpdateMesh(GetCurrentStyle());
        m_MeshNeedsUpdate = false;
    }
    void UIElement::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        if(m_Background)
            m_Background->Render(PrimType, ObjectsRendered);
    }
    void UIElement::_Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        Render(PrimType, ObjectsRendered);
        for(i32 i = 0;i < m_Children.size();i++) m_Children[i]->_Render(PrimType, ObjectsRendered);
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

    UIRenderRectangle::UIRenderRectangle(UIManager* Manager) : m_Render(0), m_Manager(Manager)
    {}

    UIRenderRectangle::~UIRenderRectangle()
    {
        if(m_Render)
            m_Manager->m_Renderer->Destroy(m_Render);
    }

    void UIRenderRectangle::UpdateMesh(UIElementStyle* Style)
    {
        if(!m_Render)
            m_Render = m_Manager->m_Renderer->CreateRenderObject(ROT_MESH);
        Material* Mat = m_Manager->m_Renderer->CreateMaterial();

        if(!Style->GetBackgroundImage())
        {
            Mat->SetShader   (m_Manager->GetDefaultShader());
        } else {
            Mat->SetShader   (m_Manager->GetDefaultTextureShader());
        }

        //TODO Border radius and the like
        Mesh* m = new Mesh();
        f32 v[18] =
        {
            0,0,0,
            1,0,0,
            0,1,0,
            
            0,1,0,
            1,0,0,
            1,1,0,
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

        m_Render->SetMesh(m, Mat);
    }
    void UIRenderRectangle::UpdateMaterial(UIElementStyle* Style)
    {
        if(!m_Render)
            UpdateMesh(Style);
        m_Render->GetMaterial()->SetDiffuse(Style->GetBackgroundColor());
        if(Style->GetBackgroundImage())
            m_Render->GetMaterial()->SetMap(Material::MT_DIFFUSE, Style->GetBackgroundImage());
    }
    void UIRenderRectangle::UpdateTransform(UIElementStyle* Style) // TODO Make the style contain transform info as well or... something
    {
        if(!m_Render)
            UpdateMesh(Style);

        m_Render->SetTransform(Translation(Style->GetPosition()) * Scale(Vec3(Style->GetSize(), 1))); // TODO Detect changes from m_Properties
    }

    void UIRenderRectangle::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
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
            
            m_Manager->m_Renderer->m_Stats.VisibleObjects++;
            m_Manager->m_Renderer->m_Stats.VertexCount   += vc;
            m_Manager->m_Renderer->m_Stats.TriangleCount += tc;
            m_Manager->m_Renderer->m_Stats.DrawCalls     ++   ;
            
            ObjectsRendered->push_back(Obj);
        }    
        Shader->Disable();
    }
};
