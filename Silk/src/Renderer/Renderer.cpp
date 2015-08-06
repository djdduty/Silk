#include <Renderer/Renderer.h>
#include <Renderer/Mesh.h>
#include <Renderer/Texture.h>

#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>

#include <Utilities/SimplexNoise.h>
#include <UI/UI.h>

#include <math.h>

#define DEFAULT_TEXTURE_SIZE 32

namespace Silk
{
    Renderer::Renderer(Rasterizer* Raster,TaskManager* TaskMgr) : m_Raster(Raster), m_TaskManager(TaskMgr), m_UIManager(0)
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
        m_Stats.DrawCalls = 0;
        m_Stats.VertexCount = m_Stats.TriangleCount = 0;
        
        m_EngineUniforms   = m_Raster->CreateUniformBuffer(ShaderGenerator::IUT_ENGINE_UNIFORMS);
        m_RendererUniforms = new RenderUniformSet(this);
        
        UpdateDefaultTexture();
        
        m_ShaderGenerator = new ShaderGenerator(this);
        
        m_Configuration = new Configuration();
        
        m_Configuration->SetRootName("Forward Renderer");
        {
            m_Configuration->AddNode("Max Lights")->Initialize(ConfigValue::VT_I32,&m_Prefs.MaxLights);
            m_Configuration->AddNode("Statistics sample duration")->Initialize(ConfigValue::VT_F32,&m_Prefs.AverageSampleDuration);
            
            ConfigNode* Cull = m_Configuration->AddNode("Culling");
            {
                Cull->AddNode("Minimum objects for multi-threaded culling")->Initialize(ConfigValue::VT_I32,&m_Prefs.MinObjectCountForMultithreadedCulling);
                Cull->AddNode("Minimum objects for multi-threaded instance transform sync")->Initialize(ConfigValue::VT_I32,&m_Prefs.MinObjectCountForMultithreadedTransformSync);
            }
        }
    }

    Renderer::~Renderer() 
    {
        if(m_Scene) delete m_Scene;
        m_Raster->Destroy(m_DefaultTexture);
        m_Raster->Destroy(m_EngineUniforms);
        
        delete m_RendererUniforms;
        delete m_ShaderGenerator;
        delete m_Configuration;
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
    void Renderer::Render(PRIMITIVE_TYPE PrimType)
    {
        Scalar dt = m_FrameTimer;
        if(dt <= 0) dt = 1.0f / 60.0f;
        m_FrameTimer.Stop();
        m_FrameTimer.Start();
        
        if(m_DefaultTextureNeedsUpdate && m_Stats.FrameID % 1 == 0) UpdateDefaultTexture();
        UpdateUniforms(); //Automatically passed to shaders that require render uniforms
        
        /*
         * Reset frame statistics
         */
        m_Stats.DrawCalls = m_Stats.VisibleObjects = m_Stats.VertexCount = m_Stats.TriangleCount = 0;
        
        /* Culling */
        CullingResult* CullResult = m_Scene->PerformCulling();
        
        /* artificial light selection (no light culling yet) */
        SilkObjectVector Lights = m_Scene->GetObjectList()->GetLightList();
        for(i32 i = 0;i < Lights.size();i++) CullResult->m_VisibleObjects->AddObject(Lights[i]);
        
        /* Render objects */
        RenderObjects(CullResult->m_VisibleObjects,PrimType);
        
        /* Render UI */
        m_UIManager->Render(PrimType);
        
        /* Compute new averages */
        m_Stats.FrameID++;
        
        i32 sCount = m_Prefs.AverageSampleDuration / dt;
        m_Stats.FrameRate = 1.0f / dt;
        m_Stats.MultithreadedCullingEfficiency = CullResult->m_Efficiency;
        
        m_Stats.AverageDrawCalls                     .SetSampleCount(sCount);
        m_Stats.AverageVertexCount                   .SetSampleCount(sCount);
        m_Stats.AverageTriangleCount                 .SetSampleCount(sCount);
        m_Stats.AverageVisibleObjects                .SetSampleCount(sCount);
        m_Stats.AverageFramerate                     .SetSampleCount(sCount);
        m_Stats.AverageMultithreadedCullingEfficiency.SetSampleCount(sCount);
        
        m_Stats.AverageDrawCalls                     .AddSample(m_Stats.DrawCalls                     );
        m_Stats.AverageVertexCount                   .AddSample(m_Stats.VertexCount                   );
        m_Stats.AverageTriangleCount                 .AddSample(m_Stats.TriangleCount                 );
        m_Stats.AverageVisibleObjects                .AddSample(m_Stats.VisibleObjects                );
        m_Stats.AverageFramerate                     .AddSample(m_Stats.FrameRate                     );
        m_Stats.AverageMultithreadedCullingEfficiency.AddSample(m_Stats.MultithreadedCullingEfficiency);
        
        
        delete CullResult;
    }
    void Renderer::RenderObjects(ObjectList *List,PRIMITIVE_TYPE PrimType)
    {
        SilkObjectVector Lights = List->GetLightList();
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
        
        i32 ShaderCount = List->GetShaderCount();
        
        SilkObjectVector MeshesRendered;
        i32 ActualObjectCount = 0;
        i32 VertexCount       = 0;
        i32 TriangleCount     = 0;
        
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader* Shader = List->GetShader(i);
            if(!Shader) continue;
            
            Shader->Enable();
            
            SilkObjectVector Meshes = List->GetShaderMeshList(i);
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
                    i32 vc = Obj->m_Mesh->GetVertexCount();
                    i32 tc = 0;
                    if(PrimType == PT_TRIANGLES     ) tc = vc / 3;
                    if(PrimType == PT_TRIANGLE_STRIP
                    || PrimType == PT_TRIANGLE_FAN  ) tc = vc - 2;
                    
                    if(Obj->IsInstanced())
                    {
                        i32 InstanceCount = Obj->GetMesh()->m_VisibleInstanceCount;
                        ActualObjectCount +=      InstanceCount;
                        VertexCount       += vc * InstanceCount;
                        TriangleCount     += tc * InstanceCount;
                    }
                    else
                    {
                        ActualObjectCount++;
                        VertexCount   += vc;
                        TriangleCount += tc;
                    }
                    MeshesRendered.push_back(Obj);
                }
            }
            
            Shader->Disable();
        }
        
        for(i32 i = 0;i < MeshesRendered.size();i++)
        {
            MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();
        }
        
        m_Stats.DrawCalls      += MeshesRendered.size();
        m_Stats.VisibleObjects += ActualObjectCount;
        m_Stats.VertexCount    += VertexCount;
        m_Stats.TriangleCount  += TriangleCount;
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
