#include <Renderer/Renderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>
#include <Raster/Raster.h>

#include <Raster/OpenGL/OpenGLShader.h>

#include <Utilities/SimplexNoise.h>

#include <math.h>

#define DEFAULT_TEXTURE_SIZE 64

namespace Silk
{
    Renderer::Renderer(Rasterizer* Raster) : 
        m_ObjectList(new ObjectList()), m_Raster(Raster)
    {
        m_DefaultTexture   = m_Raster->CreateTexture();
        m_DefaultTexture->CreateTexture(DEFAULT_TEXTURE_SIZE,DEFAULT_TEXTURE_SIZE);
        m_DefaultTexture->InitializeTexture();
        m_DefaultTexturePhase = 0.0f;
        
        m_DoRecompileAllShaders = false;
        m_Prefs.MaxLights = 8;
        m_Stats.FrameID = 0;
        
        m_ActiveCamera = 0;
        
        m_EngineUniforms   = m_Raster->CreateUniformBuffer(ShaderGenerator::IUT_ENGINE_UNIFORMS  );
        m_RendererUniforms = new RenderUniformSet(this);
        
        UpdateDefaultTexture();
        
        m_ActiveCamera = 0;
    }

    Renderer::~Renderer() 
    {
        m_ObjectList->Clear();
        delete m_ObjectList;
        
        m_Raster->Destroy(m_DefaultTexture);
        m_Raster->Destroy(m_EngineUniforms);
        
        delete m_RendererUniforms;
    }
    Texture* Renderer::GetDefaultTexture()
    {
        m_DefaultTextureNeedsUpdate = true;
        return m_DefaultTexture;
    }
    void Renderer::UpdateUniforms()
    {
        m_RendererUniforms->UpdateUniforms();
    }
    void Renderer::Render(i32 PrimType)
    {
        if(m_DefaultTextureNeedsUpdate && m_Stats.FrameID % 5 == 0) UpdateDefaultTexture();
        UpdateUniforms(); //Automatically passed to shaders that require render uniforms
        
        SilkObjectVector Lights = m_ObjectList->GetLightList();
        
        /*
         * To do:
         * Determine which lights affect which objects using a scene octree
         * then call Object->GetUniformSet()->SetLights(ObjectLightVector);
         */
        
        i32 ShaderCount = m_ObjectList->GetShaderCount();
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader*          Shader = m_ObjectList->GetShader        (i);
            if(!Shader) continue;
            
            Shader->Enable();
            
            SilkObjectVector Meshes = m_ObjectList->GetShaderMeshList(i);
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    //Pass material uniforms
                    Shader->UseMaterial(Obj->GetMaterial());
                    
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
                    
                    Obj->m_ObjectIdentifier->Render(PrimType,0,Count);
                }
            }
            
            Shader->Disable();
        }
        
        m_UpdatedObjects.clear();
        
        m_Stats.FrameID++;
    }

    RenderObject* Renderer::CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene)
    {
        RenderObject* Object = new RenderObject(Rot, this, m_Raster->CreateObjectIdentifier());
        if(!Object)
            return nullptr;

        if(AddToScene)
            AddRenderObject(Object);

        return Object;
    }
    Material* Renderer::CreateMaterial()
    {
        //More here in the future
        return new Material(this);
    }
    void Renderer::Destroy(Material *Mat)
    {
        delete Mat;
    }
    void Renderer::Destroy(RenderObject* Obj)
    {
        RemoveRenderObject(Obj);
        delete Obj;
    }
    
    void Renderer::AddRenderObject(RenderObject *Object)
    {
        if(!Object) return;
            
        Object->m_ListIndex = m_ObjectList->AddObject(Object);
        Object->m_List = m_ObjectList;
    }
    
    void Renderer::UpdateDefaultTexture()
    {
        m_DefaultTextureNeedsUpdate = false;
        for(i32 x = 0;x < DEFAULT_TEXTURE_SIZE;x++)
        {
            for(i32 y = 0;y < DEFAULT_TEXTURE_SIZE;y++)
            {
                f32 r = 1.0f;
                f32 nx = (((f32)x) / ((f32)DEFAULT_TEXTURE_SIZE) + (cos(m_DefaultTexturePhase * 0.1f) * r)) * 20.0f;
                f32 ny = (((f32)y) / ((f32)DEFAULT_TEXTURE_SIZE) + (sin(m_DefaultTexturePhase * 0.1f) * r)) * 20.0f;
                f32 xZoom = sin(m_DefaultTexturePhase * 2.0f) * 10.0f;
                f32 yZoom = cos(m_DefaultTexturePhase * 2.0f) * 10.0f;
                f32 n = octave_noise_3d(4,0.4f,1.0f,nx / (20.0f + xZoom),ny / (20.0f + yZoom),m_DefaultTexturePhase * 2.0f);
                m_DefaultTexture->SetPixel(Vec2(x,y),Vec4(ColorFunc(n + m_DefaultTexturePhase * 0.2f),1.0));
            }
        }
        m_DefaultTexturePhase += 0.025f;
        
        m_DefaultTexture->UpdateTexture();
    }
};
