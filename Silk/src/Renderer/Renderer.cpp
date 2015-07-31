#include <Renderer/Renderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>
#include <Raster/Raster.h>

#include <Raster/OpenGL/OpenGLShader.h>

#include <Utilities/SimplexNoise.h>

#include <math.h>

#define DEFAULT_TEXTURE_SIZE 32

namespace Silk
{
    Renderer::Renderer(Rasterizer* Raster,TaskManager* TaskMgr) : m_Raster(Raster), m_TaskManager(TaskMgr)
    {
        m_DefaultTexture   = m_Raster->CreateTexture();
        m_DefaultTexture->CreateTexture(DEFAULT_TEXTURE_SIZE,DEFAULT_TEXTURE_SIZE);
        m_DefaultTexture->InitializeTexture();
        m_DefaultTexturePhase = 0.0f;
        
        m_Scene = new Scene(this);
        
        m_DoRecompileAllShaders = false;
        m_Prefs.MaxLights = 8;
        m_Prefs.AverageSampleDuration = 5.0f;
        m_Prefs.MinObjectCountForMultithreadedCulling = 1000;
        m_Prefs.MinObjectCountForMultithreadedTransformSync = 1000;
        
        m_Stats.FrameID = 0;
        m_Stats.VisibleObjects = 0;
        m_Stats.FrameRate = 0.0f;
        
        m_EngineUniforms   = m_Raster->CreateUniformBuffer(ShaderGenerator::IUT_ENGINE_UNIFORMS);
        m_RendererUniforms = new RenderUniformSet(this);
        
        UpdateDefaultTexture();
    }

    Renderer::~Renderer() 
    {
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
        Scalar dt = m_FrameTimer;
        if(dt <= 0) dt = 1.0f / 60.0f;
        m_FrameTimer.Stop();
        m_FrameTimer.Start();
        
        if(m_DefaultTextureNeedsUpdate && m_Stats.FrameID % 1 == 0) UpdateDefaultTexture();
        UpdateUniforms(); //Automatically passed to shaders that require render uniforms
        
        CullingResult* CullResult = m_Scene->PerformCulling();
        
        SilkObjectVector Lights = m_Scene->GetObjectList()->GetLightList();
        std::vector<Light*> LightsVector;
        for(i32 i = 0; i < Lights.size(); i++)
        {
            LightsVector.push_back(Lights[i]->GetLight());
            Mat4 T = Lights[i]->GetTransform();
           
            Lights[i]->GetLight()->m_Position  = Vec4(T.x.w,
                                                      T.y.w,
                                                      T.z.w,
                                                      1.0f);
            
            Lights[i]->GetLight()->m_Direction = Vec4(T.x.z,
                                                      T.y.z,
                                                      T.z.z,
                                                      1.0f);
        }

        /*
         * To do:
         * Determine which lights affect which objects using a scene octree
         * then call Object->GetUniformSet()->SetLights(ObjectLightVector);
         */
        
        i32 ShaderCount = CullResult->m_VisibleObjects->GetShaderCount();
        
        SilkObjectVector MeshesRendered;
        i32 ActualObjectCount = 0;
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader* Shader = CullResult->m_VisibleObjects->GetShader(i);
            if(!Shader) continue;
            
            Shader->Enable();
            
            SilkObjectVector Meshes = CullResult->m_VisibleObjects->GetShaderMeshList(i);
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
                if(Obj->IsInstanced() && Obj->m_InstanceIndex != 0) continue;
            
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    Obj->GetUniformSet()->SetLights(LightsVector);
                    
                    //Pass material uniforms
                    Material* Mat = Obj->GetMaterial();
                    if(Mat->HasUpdated()) Shader->UseMaterial(Obj->GetMaterial());
                    
                    //Pass object uniforms
                    if(Shader->UsesUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS))
                    {
                        if(Obj->IsInstanced())
                        {
                            Mat4 tmp = Obj->GetTransform();
                            Obj->SetTransform(Mat4::Identity);
                            Obj->UpdateUniforms();
                            Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
                            Obj->SetTransform(tmp);
                        }
                        else
                        {
                            Obj->UpdateUniforms();
                            Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
                        }
                    }
                    
                    //To do: Batching
                    i32 Count = 0;
                    if(Obj->m_Mesh->IsIndexed()) Count = Obj->m_Mesh->GetIndexCount();
                    else Count = Obj->m_Mesh->GetVertexCount();
                        
                    Obj->m_Object->Render(Obj,PrimType,0,Count);
                    
                    if(Obj->IsInstanced()) ActualObjectCount += Obj->GetMesh()->m_VisibleInstanceCount;
                    else ActualObjectCount++;
                    MeshesRendered.push_back(Obj);
                }
            }
            
            Shader->Disable();
        }
        
        for(i32 i = 0;i < MeshesRendered.size();i++)
        {
            MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();
        }
        
        
        m_Stats.FrameID++;
        
        i32 sCount = m_Prefs.AverageSampleDuration / dt;
        
        m_Stats.DrawCalls = MeshesRendered.size();
        m_Stats.AverageDrawCalls.SetSampleCount(sCount);
        m_Stats.AverageDrawCalls.AddSample(m_Stats.DrawCalls);
        
        m_Stats.VisibleObjects = ActualObjectCount;
        m_Stats.AverageVisibleObjects.SetSampleCount(sCount);
        m_Stats.AverageVisibleObjects.AddSample(m_Stats.VisibleObjects);
        
        m_Stats.FrameRate = 1.0f / dt;
        m_Stats.AverageFramerate.SetSampleCount(sCount);
        m_Stats.AverageFramerate.AddSample(m_Stats.FrameRate);
        
        m_Stats.MultithreadedCullingEfficiency = CullResult->m_Efficiency;
        m_Stats.AverageMultithreadedCullingEfficiency.SetSampleCount(sCount);
        m_Stats.AverageMultithreadedCullingEfficiency.AddSample(m_Stats.MultithreadedCullingEfficiency);
        
        delete CullResult;
    }

    RenderObject* Renderer::CreateRenderObject(RENDER_OBJECT_TYPE Rot,bool AddToScene)
    {
        RenderObject* Object = new RenderObject(Rot, this, m_Raster->CreateObject());
        
        if(!Object)
            return nullptr;

        if(AddToScene) m_Scene->AddRenderObject(Object);

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
        m_Scene->RemoveRenderObject(Obj);
        delete Obj;
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
