#include <UI/UIContent.h>
#include <UI/UI.h>

namespace Silk
{
    UIRenderContent::UIRenderContent(UIManager* Manager, UIElementStyle* Style) : m_Manager(Manager), m_Style(Style), m_Render(0)
    {

    }

    UIRenderContent::~UIRenderContent()
    {
        if(m_Render)
            m_Manager->m_Renderer->Destroy(m_Render);
    }

    void UIRenderContent::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
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


    UIRenderRectangle::UIRenderRectangle(UIManager* Manager, UIElementStyle* Style) : UIRenderContent(Manager, Style)
    {}

    void UIRenderRectangle::UpdateMesh()
    {
        if(!m_Render)
            m_Render = m_Manager->GetRenderer()->CreateRenderObject(ROT_MESH);
        Material* Mat = m_Manager->GetRenderer()->CreateMaterial();

        if(!m_Style->GetBackgroundImage())
        {
            Mat->SetShader   (m_Manager->GetDefaultShader());
        } else {
            Mat->SetShader   (m_Manager->GetDefaultTextureShader());
        }
        Vec2 Size = m_Style->GetSize();
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

        m_Render->SetMesh(m, Mat);
    }
    void UIRenderRectangle::UpdateMaterial()
    {
        if(!m_Render)
            UpdateMesh();
        m_Render->GetMaterial()->SetDiffuse(m_Style->GetBackgroundColor());
        if(m_Style->GetBackgroundImage())
            m_Render->GetMaterial()->SetMap(Material::MT_DIFFUSE, m_Style->GetBackgroundImage());
    }
    void UIRenderRectangle::UpdateTransform()
    {
        if(!m_Render)
            UpdateMesh();

        m_Render->SetTransform(Translation(m_Style->GetPosition()));
    }
}