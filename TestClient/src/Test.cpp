#include <Test.h>
#include <Window.h>
#include <LodePNG.h>

namespace TestClient
{
    Test::Test()
    {
    }
    Test::~Test()
    {
        for(i32 i = 0;i < m_Lights.size();i++)
        {
            delete m_Lights[i]->GetLight();
            m_Renderer->Destroy(m_Lights[i]);
        }
        m_Lights.clear();
        m_LightMeshes.clear();
        
        for(i32 i = 0;i < m_Meshes.size();i++)
        {
            m_Renderer->Destroy(m_Meshes[i]);
        }
        m_Meshes.clear();
        
        for(i32 i = 0;i < m_Materials.size();i++)
        {
            if(m_Materials[i]->GetMap(Material::MT_DIFFUSE )) m_Materials[i]->GetMap(Material::MT_DIFFUSE )->Destroy();
            if(m_Materials[i]->GetMap(Material::MT_NORMAL  )) m_Materials[i]->GetMap(Material::MT_NORMAL  )->Destroy();
            if(m_Materials[i]->GetMap(Material::MT_PARALLAX)) m_Materials[i]->GetMap(Material::MT_PARALLAX)->Destroy();
            if(m_Materials[i]->GetShader()) m_Renderer->GetRasterizer()->Destroy(m_Materials[i]->GetShader());
            m_Renderer->Destroy(m_Materials[i]);
        }
        m_Materials.clear();
        
        delete m_ObjLoader;
        m_ObjLoader = 0;
        
        m_Renderer->GetScene()->SetActiveCamera(0);
        if(m_Camera         ) delete m_Camera         ;
        if(m_ShaderGenerator) delete m_ShaderGenerator;
        if(m_UIManager      ) delete m_UIManager      ;
        if(m_Renderer       ) delete m_Renderer       ;
        if(m_Window         ) delete m_Window         ;
        if(m_TaskManager    ) delete m_TaskManager    ;
    }
    void Test::Init()
    {
        m_Window = new Window();
        #ifdef __APPLE__
        if(m_Window->Create(Vec2(400,300),GetTestName(),true))
        #else
        if(m_Window->Create(Vec2(800,600),GetTestName(),true))
        #endif
        {
            m_Rasterizer = m_Window->GetRasterizer();
            ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
            m_Window->PollEvents();

            m_TaskManager = new TaskManager();
            m_Renderer = new Renderer(m_Rasterizer,m_TaskManager);
            m_Rasterizer->SetRenderer(m_Renderer);
        
            m_ShaderGenerator = new ShaderGenerator(m_Renderer);
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            
            m_Camera = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
            m_Renderer->GetScene()->SetActiveCamera(m_Camera);
            m_Renderer->GetScene()->SetCullingAlgorithm(new BruteForceCullingAlgorithm(m_Renderer->GetScene(),m_TaskManager));
            
            m_ElapsedTime = m_LastElapsedTime = m_Window->GetElapsedTime();
            
            m_DeltaTime = 1.0f / 60.0f;
            m_FramePrintTime = 0.0f;
            m_FramePrintInterval = 1.5f;
            
            m_Rasterizer->SetClearColor(Vec4(0.1f,0.1f,0.1f,1.0f));
            
            m_Window->PollEvents();
            m_Rasterizer->ClearActiveFramebuffer();
            
            m_DoShutdown = false;
            
            m_ObjLoader = new ObjLoader();
            
            m_UIManager = new UIManager(m_Renderer);
            m_Renderer->SetUIManager(m_UIManager);
            m_UIManager->Initialize();
        }
        else
        {
            m_DoShutdown      = true;
            m_Rasterizer      = 0;
            m_Renderer        = 0;
            m_ShaderGenerator = 0;
            delete m_Window;
            delete m_TaskManager;
            m_Window          = 0;
        }
        Initialize();
    }
    Byte* Test::Load(const char* File,i64 *OutSize)
    {
        FILE* fp = fopen(File,"rb");
        if(!fp) { ERROR("Unable to open file <%s>.\n",File); return 0; }
        
        fseek(fp,0,SEEK_END);
        i64 Sz = ftell(fp);
        rewind(fp);
        
        if(OutSize) (*OutSize) = Sz + 1;
        
        Byte* Data = new Byte[Sz + 1];
        if(fread(Data,Sz,1,fp) != 1)
        {
            ERROR("Unable to load %lld bytes from file <%s>.\n",Sz,File);
            fclose(fp);
            delete [] Data;
            return 0;
        }
        Data[Sz] = 0;
        
        fclose(fp);
        
        return Data;
    }
    bool  Test::Save(const char* File,Byte* Data,i64 Size)
    {
        FILE* fp = fopen(File,"wb");
        if(!fp) { ERROR("Unable to open file <%s>.\n",File); return false; }
        
        if(fwrite(Data,Size,1,fp) != 1)
        {
            ERROR("Unable to write %lld bytes to file <%s>.\n",Size,File);
            fclose(fp);
            return false;
        }
        
        fclose(fp);
        
        return true;
    }
    RenderObject* Test::AddLight(LightType Type,const Vec3& Pos)
    {
        RenderObject* LObj = m_Renderer->CreateRenderObject(ROT_LIGHT,false);
        Light* L = new Light(Type);
        
        LObj->SetLight(L);
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;
        LObj->SetTransform(Translation(Pos));
        
        m_Renderer->GetScene()->AddRenderObject(LObj);
        m_Lights.push_back(LObj);
        return LObj;
    }
    i32 Test::AddMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count)
    {
        m_ObjLoader->Load(const_cast<CString>(Path));
        m_ObjLoader->ComputeTangents();
        
        Mesh* M = new Mesh();
        M->SetVertexBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetPositions()));
        M->SetNormalBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetNormals  ()));
        M->SetTangentBuffer (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTangents ()));
        M->SetTexCoordBuffer(m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTexCoords()));
        
        RenderObject* Obj = 0;
        i32 First = m_Meshes.size();
        for(i32 i = 0;i < Count;i++)
        {
            Obj = m_Renderer->CreateRenderObject(ROT_MESH,false);
            Obj->SetMesh(M,Mat);
            m_Renderer->GetScene()->AddRenderObject(Obj);
            
            Obj->SetTransform(Translation(Pos));
            Obj->SetTextureTransform(Mat4::Identity);
            m_Meshes.push_back(Obj);
        }
        if(Count > 1) return First;
        return m_Meshes.size() - 1;
    }
    Texture* Test::LoadTexture(const char *Path)
    {
        if(!Path) return 0;
        vector<u8> Pixels;
        u32 w,h;
        static f32 Inv255 = 1.0f / 255.0f;
        
        lodepng::decode(Pixels,w,h,Path);
        if(Pixels.size() == 0) return 0;
        
        Texture* Tex = m_Rasterizer->CreateTexture();
        Tex->CreateTexture(w,h);
        for(i32 x = 0;x < w;x++)
        {
            for(i32 y = 0;y < h;y++)
            {
                i32 Idx = (y * (w * 4)) + (x * 4);
                
                Vec4 C;
                C.x = f32(Pixels[Idx + 0]) * Inv255;
                C.y = f32(Pixels[Idx + 1]) * Inv255; 
                C.z = f32(Pixels[Idx + 2]) * Inv255;
                C.w = f32(Pixels[Idx + 3]) * Inv255;
                
				swap(C.x,C.z);
                
                Tex->SetPixel(Vec2(x,(h-1) - y),C);
            }
        }
        Tex->UpdateTexture();
        return Tex;
    }
    Material* Test::AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse,const char* Normal,const char* Parallax)
    {
        Material* Mat = m_Renderer->CreateMaterial();
        Texture* D = LoadTexture(Diffuse );
        Texture* N = LoadTexture(Normal  );
        Texture* P = LoadTexture(Parallax);
        
        if(D) { Mat->SetMap(Material::MT_DIFFUSE ,D); D->Destroy(); }
        if(N) { Mat->SetMap(Material::MT_NORMAL  ,N); N->Destroy(); }
        if(P) { Mat->SetMap(Material::MT_PARALLAX,P); P->Destroy(); }
        
        if(LightingMode != ShaderGenerator::LM_FLAT)
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL  ,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION,true);
            m_ShaderGenerator->SetAllowInstancing(false);
            
            if(Normal || Parallax)
            {
                m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TANGENT ,true);
                m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TEXCOORD,true);
                m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_NORMAL  ,true);
                
                m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT ,true);
                m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
                m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
            }
        }
        else
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL  ,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,false);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TANGENT ,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT ,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION,false);
            m_ShaderGenerator->SetAllowInstancing(true);
        }
        
        if(Diffuse)
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
        }
        else
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,false);
        }
        
        m_ShaderGenerator->SetTextureInput(Material::MT_DIFFUSE ,D != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_NORMAL  ,N != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_PARALLAX,P != 0);
        m_ShaderGenerator->SetLightingMode(LightingMode);

        Mat->SetShader(m_ShaderGenerator->Generate());
        
        Mat->SetSpecular(Vec4(0,0,0,0));
        Mat->SetShininess(0.0f);
        
        m_Materials.push_back(Mat);
        return Mat;
    }
    
    Scalar ConvertUnit(Scalar n)
    {
        if(n >= 1000.0f && n < 1000000.0f) return n * 0.001f;
        else if(n >= 1000000.0f) return n * 0.000001f;
        return n * 1.0f;
    }
    string GetUnit(Scalar n)
    {
        if(n >= 1000.0f && n < 1000000.0f) return "k";
        else if(n >= 1000000.0f) return "M";
        return " ";
    }
    bool Test::IsRunning()
    {
        if(m_Renderer->GetRenderStatistics().FrameID > 0)
        {
            /* End previous frame */
            m_Window->SwapBuffers();
        }
        
        /* Start new frame */
        if(!m_Window->GetCloseRequested() && !m_DoShutdown)
        {
            m_TaskManager->EndFrame  ();
            m_TaskManager->BeginFrame();
            
            m_Window->PollEvents();
            m_Rasterizer->ClearActiveFramebuffer();
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            m_Camera->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
            
            f64 x,y;
            glfwGetCursorPos(m_Window->GetWindow(),&x,&y);
            
            GLint Viewport[4];
            glGetIntegerv(GL_VIEWPORT,Viewport);
            
            Mat4 v = m_Camera->GetTransform();
            Mat4 p = m_Camera->GetProjection();
            
            m_CursorRay = UnProject(Vec3(x,y,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
            
            m_LastElapsedTime = m_ElapsedTime;
            m_ElapsedTime     = m_Window->GetElapsedTime();
            m_DeltaTime       = m_ElapsedTime - m_LastElapsedTime;
            
            m_FramePrintTime += m_DeltaTime;
            if(m_FramePrintTime > m_FramePrintInterval)
            {
                const Renderer::RenderStats& Stats = m_Renderer->GetRenderStatistics();
                f32 vc    = Stats.VertexCount   ;
                f32 tc    = Stats.TriangleCount ;
                f32 dc    = Stats.DrawCalls     ;
                f32 fr    = Stats.FrameRate     ;
                f32 vo    = Stats.VisibleObjects;
                f32 ceff  = (Stats.MultithreadedCullingEfficiency - 1.0f) * 100.0f;
                
                f32 avc   = Stats.AverageVertexCount   .GetAverage();
                f32 atc   = Stats.AverageTriangleCount .GetAverage();
                f32 adc   = Stats.AverageDrawCalls     .GetAverage();
                f32 afr   = Stats.AverageFramerate     .GetAverage();
                f32 avo   = Stats.AverageVisibleObjects.GetAverage();
                f32 aceff = (Stats.AverageMultithreadedCullingEfficiency.GetAverage() - 1.0f) * 100.0f;
                if(ceff  < 0.0f) ceff = 0.0f;
                if(aceff < 0.0f) aceff = 0.0f;
                
                /* * * * * * * * * * * * * *\
                 *     /.-'       `-.\     *
                 *    //             \\    *
                 *   /j_______________j\   *
                 *  /o.-==-. .-. .-==-.o\  *
                 *  ||      )) ((      ||  *
                 *   \\____//   \\____//   *
                 *    `-==-'     `-==-'    *
                \* * * * * * * * * * * * * */
                
                printf("+-------------(Render Statistics)-------------+\n");
                printf("| Frame ID    : %10lld f  " "                |\n",Stats.FrameID);
                printf("| Run time    : %10.3f s  " "                |\n",m_ElapsedTime);
                
                                  printf("| Frame Rate  : %8.3f %sHz "     "(A: %7.2f" " %sHz)"   " |\n",     ConvertUnit(fr  ),GetUnit(fr  ).c_str(),ConvertUnit(afr  ),GetUnit(afr  ).c_str());
                if(dc >= 1000.0f) printf("| Draw calls  : %8.3f %s   "     "(A: %7.2f" " %s  )"   " |\n",     ConvertUnit(dc  ),GetUnit(dc  ).c_str(),ConvertUnit(adc  ),GetUnit(adc  ).c_str());
                else              printf("| Draw calls  : %8d     "        "(A: %7.2f" " %s  )"   " |\n",(i32)ConvertUnit(dc  )                      ,ConvertUnit(adc  ),GetUnit(adc  ).c_str());
                if(vc >= 1000.0f) printf("| Vertices    : %8.3f %s   "     "(A: %7.2f" " %s  )"   " |\n",     ConvertUnit(vc  ),GetUnit(vc  ).c_str(),ConvertUnit(avc  ),GetUnit(avc  ).c_str());
                else              printf("| Vertices    : %8d     "        "(A: %7.2f" " %s  )"   " |\n",(i32)ConvertUnit(vc  )                      ,ConvertUnit(avc  ),GetUnit(avc  ).c_str());
                if(tc >= 1000.0f) printf("| Triangles   : %8.3f %s   "     "(A: %7.2f" " %s  )"   " |\n",     ConvertUnit(tc  ),GetUnit(tc  ).c_str(),ConvertUnit(atc  ),GetUnit(atc  ).c_str());
                else              printf("| Triangles   : %8d     "        "(A: %7.2f" " %s  )"   " |\n",(i32)ConvertUnit(tc  )                      ,ConvertUnit(atc  ),GetUnit(atc  ).c_str());
                if(vo >= 1000.0f) printf("| Object Count: %8.3f %s   "     "(A: %7.2f" " %s  )"   " |\n",     ConvertUnit(vo  ),GetUnit(vo  ).c_str(),ConvertUnit(avo  ),GetUnit(avo  ).c_str());
                else              printf("| Object Count: %8d     "        "(A: %7.2f" " %s  )"   " |\n",(i32)ConvertUnit(vo  )                      ,ConvertUnit(avo  ),GetUnit(avo  ).c_str());
                                  printf("| Cull Effic. : %8.3f %s \%% "   "(A: %7.2f" " %s \%%)" " |\n",     ConvertUnit(ceff),GetUnit(ceff).c_str(),ConvertUnit(aceff),GetUnit(aceff).c_str());
                printf("+---------(Sample Duration: %6.2f s)---------+\n",m_Renderer->GetAverageSampleDuration());
                
                m_FramePrintTime  = 0.0f;
            }
            return true;
        }
        return false;
    }
};