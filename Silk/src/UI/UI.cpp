#include <UI/UI.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>

namespace Silk
{
    UIManager::UIManager(Renderer* r) : m_Renderer(r), m_ViewScale(1.0f,1.0f), m_Resolution(0.0f,0.0f), m_ViewNeedsUpdate(true),
                                        m_Camera(0), m_View(0), m_DefaultShader(0), m_DefaultTextureShader(0), m_DefaultTextShader(0),
                                        m_RenderUniforms(0)
    {
    }
    UIManager::~UIManager()
    {
        if(m_Camera) delete m_Camera;
    }
    
    void UIManager::Initialize()
    {
        m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        Vec2 hRes = m_Resolution * 0.5f;
        m_Camera = new Camera(-hRes.x,hRes.x,-hRes.y,hRes.y,0.0f,1.0f);
        m_View = m_Renderer->GetRasterizer()->CreateTexture();
        m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
        
        ShaderGenerator* Gen = m_Renderer->GetShaderGenerator();
        Gen->Reset();
        
        Gen->SetShaderVersion(330);
        Gen->SetAllowInstancing(false);
        Gen->SetAllowInstancedTextureMatrix(false);
        Gen->SetTextureInput(Material::MT_DIFFUSE,true);
        
        Gen->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        Gen->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        Gen->SetUniformInput(ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);

        Gen->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
        Gen->SetAttributeInput (ShaderGenerator::IAT_COLOR   ,true);
        Gen->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
        
        Gen->SetAttributeOutput(ShaderGenerator::IAT_COLOR   ,true);
        Gen->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
        Gen->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
        
        Gen->SetLightingMode(ShaderGenerator::LM_FLAT);
        
        m_DefaultShader = Gen->Generate();
        
        //m_DefaultTextureShader = Gen->Generate();
        
        //m_DefaultTextShader = Gen->Generate();
    }
    
    void UIManager::Render(PRIMITIVE_TYPE PrimType)
    {
        //Update texture and projection if resolution changes
        Vec2 cRes = m_Renderer->GetRasterizer()->GetContext()->GetResolution();;
        if(m_Resolution.x != cRes.x || m_Resolution.y != cRes.y)
        {
            m_Resolution = cRes;
            m_Camera->SetOrthographic(m_Resolution);
            m_ViewNeedsUpdate = true;
            m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
            m_View->UpdateTexture();
        }
        
        if(!m_ViewNeedsUpdate) return;
        //m_ViewNeedsUpdate = false;
        
        //Set active camera
        Camera* Cam = m_Renderer->GetScene()->GetActiveCamera();
        m_Renderer->GetScene()->SetActiveCamera(m_Camera);
        m_Renderer->UpdateUniforms();
        
        //UI culling
        ObjectList l;
        l.SetIndexed(false);
        
        for(i32 i = 0;i < m_Elements.size();i++)
        {
            //To do: culling
            l.AddObject(m_Elements[i]->m_Render);
        }
        
        //Render all UI to texture
        m_View->EnableRTT();
        
        i32 ShaderCount = l.GetShaderCount();
        
        SilkObjectVector MeshesRendered;
        i32 ActualObjectCount = 0;
        i32 VertexCount       = 0;
        i32 TriangleCount     = 0;
        
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader* Shader = l.GetShader(i);
            if(!Shader) continue;
            
            Shader->Enable();
            
            SilkObjectVector Meshes = l.GetShaderMeshList(i);
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
            
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    //Pass material uniforms
                    Material* Mat = Obj->GetMaterial();
                    if(Mat->HasUpdated()) Shader->UseMaterial(Obj->GetMaterial());
                    
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
                    
                    m_Renderer->m_Stats.VisibleObjects++;
                    m_Renderer->m_Stats.VertexCount   += vc;
                    m_Renderer->m_Stats.TriangleCount += tc;
                    
                    MeshesRendered.push_back(Obj);
                }
            }
            
            Shader->Disable();
        }
        
        for(i32 i = 0;i < MeshesRendered.size();i++)
        {
            MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();
        }
        
        m_View->DisableRTT();
        
        m_Renderer->GetScene()->SetActiveCamera(Cam);
        
        //Do some other stuff with the view
    }
    UIElement* UIManager::CreateElement()
    {
        UIElement* Element = new UIElement(this);
        m_Elements.push_back(Element);
        Element->m_ID = m_Elements.size() - 1;
        return Element;
    }
};
