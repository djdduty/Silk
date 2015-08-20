#include <Test.h>
#include <Window.h>
#include <LodePNG.h>

namespace TestClient
{
    static Test* g_Test = 0;
    void OnCursorMove(GLFWwindow* Win,f64 x,f64 y)
    {
        if(!g_Test) return;
        g_Test->GetUI()->SetCursorPosition(Vec2(x,y));
    }
    void OnClick(GLFWwindow* Win,i32 Button,i32 Action,i32 Mods)
    {
        if(!g_Test) return;
        
        if(Button == GLFW_MOUSE_BUTTON_LEFT)
        {
                 if(Action == GLFW_PRESS  ) g_Test->GetUI()->OnButtonDown(BTN_LEFT_MOUSE);
            else if(Action == GLFW_RELEASE) g_Test->GetUI()->OnButtonUp  (BTN_LEFT_MOUSE);
        }
        else if(Button == GLFW_MOUSE_BUTTON_RIGHT)
        {
                 if(Action == GLFW_PRESS  ) g_Test->GetUI()->OnButtonDown(BTN_RIGHT_MOUSE);
            else if(Action == GLFW_RELEASE) g_Test->GetUI()->OnButtonUp  (BTN_RIGHT_MOUSE);
        }
    }
    void OnKey(GLFWwindow* Win,i32 Key,i32 ScanCode,i32 Action,i32 Mods)
    {
        if(!g_Test) return;
        /*
         * Break glass in case of game engine
         *  ____________
         * |\           \
         * | \___________\
         * | | _________ |
         * | | |   \   | |
         * | | |   \\  | |
         * | | |    \  | |
         * | | |button | |
         * | | |mapping| |
         * \ | |class  | |
         * \ | |_______| |
         *  \|___________|
         */
        
        if(Action == GLFW_PRESS)
        {
                 if(Key == GLFW_KEY_W     ) g_Test->GetUI()->OnButtonDown(BTN_MOVE_FORWARD );
            else if(Key == GLFW_KEY_S     ) g_Test->GetUI()->OnButtonDown(BTN_MOVE_BACKWARD);
            else if(Key == GLFW_KEY_A     ) g_Test->GetUI()->OnButtonDown(BTN_MOVE_LEFT    );
            else if(Key == GLFW_KEY_D     ) g_Test->GetUI()->OnButtonDown(BTN_MOVE_RIGHT   );
            else if(Key == GLFW_KEY_ESCAPE) g_Test->GetUI()->OnButtonDown(BTN_QUIT         );
        }
        else if(Action == GLFW_RELEASE)
        {
                 if(Key == GLFW_KEY_W     ) g_Test->GetUI()->OnButtonUp(BTN_MOVE_FORWARD );
            else if(Key == GLFW_KEY_S     ) g_Test->GetUI()->OnButtonUp(BTN_MOVE_BACKWARD);
            else if(Key == GLFW_KEY_A     ) g_Test->GetUI()->OnButtonUp(BTN_MOVE_LEFT    );
            else if(Key == GLFW_KEY_D     ) g_Test->GetUI()->OnButtonUp(BTN_MOVE_RIGHT   );
            else if(Key == GLFW_KEY_ESCAPE) g_Test->GetUI()->OnButtonUp(BTN_QUIT         );
        }
    }
    
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
            if(m_Materials[i]->GetShader()) m_Materials[i]->SetShader(0);
            m_Renderer->Destroy(m_Materials[i]);
        }
        m_Materials.clear();
        
        delete m_ObjLoader;
        m_ObjLoader = 0;
        
        m_Renderer->GetScene()->SetActiveCamera(0);
        if(m_Camera         ) delete m_Camera         ;
        if(m_UIManager      ) delete m_UIManager      ;
        if(m_Renderer       ) delete m_Renderer       ;
        if(m_Window         ) delete m_Window         ;
        if(m_TaskManager    ) delete m_TaskManager    ;
    }
    void Test::Init()
    {
        g_Test = this;
        m_Window = new Window();
        if(m_Window->Create(GetPreferredInitResolution(),GetTestName(),true))
        {
            m_Rasterizer = m_Window->GetRasterizer();
            ((OpenGLRasterizer*)m_Rasterizer)->SetClearBuffers(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
            m_Window->PollEvents();

            m_TaskManager = new TaskManager();
            m_Renderer = new Renderer(m_Rasterizer,m_TaskManager);
            m_Rasterizer->SetRenderer(m_Renderer);
            m_Renderer->Init();
            
            m_DebugDraw = new DebugDrawer(m_Renderer);
            m_Renderer->SetDebugDrawer(m_DebugDraw);
        
            m_ShaderGenerator = m_Renderer->GetShaderGenerator();
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            
            m_Camera = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
            m_Renderer->GetScene()->SetActiveCamera(m_Camera);
            m_Renderer->GetScene()->SetCullingAlgorithm(new BruteForceCullingAlgorithm(m_Renderer->GetScene(),m_TaskManager));
            
            m_ElapsedTime = m_LastElapsedTime = m_Window->GetElapsedTime();
            
            m_DeltaTime = 1.0f / 60.0f;
            m_FramePrintTime = 0.0f;
            m_FramePrintInterval = 1.5f;
            SetTargetFrameRate(60.0f);
            
            m_Rasterizer->SetClearColor(Vec4(0.1f,0.1f,0.1f,1.0f));
            
            m_Window->PollEvents();
            m_Rasterizer->ClearActiveFramebuffer();
            
            m_DoShutdown = m_FlyCameraEnabled = false;
            
            m_ObjLoader = new ObjLoader();
            m_UIManager = 0;
            m_Cursor    = 0;
            m_CursorMat = 0;
            m_CursorObj = 0;
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
    void Test::InitGUI()
    {
        m_UIManager = new UIManager(m_Renderer);
        m_Renderer->SetUIManager(m_UIManager);
        m_UIManager->Initialize(BTN_COUNT);
        m_UIManager->GetCamera()->SetZClipPlanes(0.0f,200.0f);
        m_UIManager->GetCamera()->SetTransform(Translation(Vec3(0,0,-100)));
        
        glfwSetInputMode          (m_Window->GetWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        glfwSetMouseButtonCallback(m_Window->GetWindow(),OnClick     );
        glfwSetCursorPosCallback  (m_Window->GetWindow(),OnCursorMove);
        glfwSetKeyCallback        (m_Window->GetWindow(),OnKey       );
    }
    void Test::InitCursor()
    {
        if(!LoadTexture("Common/Cursor.png"))
        {
            ERROR("TestClient: Unable to initialize cursor. \"Common/Cursor.png\" not found.\n");
        }
        
        m_CursorTexIndex = m_Textures.size() - 1;
        
        Mesh* m = new Mesh();
        f32 s = 20.0f;
        f32 v[18] =
        {
            0,0,0,
            s,0,0,
            0,s,0,
            
            0,s,0,
            s,0,0,
            s,s,0,
        };
        
        s = 0.9f; //Avoid texture edge artifact that happens with filtering
        f32 t[12] =
        {
            0,0,
            s,0,
            0,s,
            
            0,s,
            s,0,
            s,s
        };
        
        m->SetVertexBuffer  (6,v);
        m->SetTexCoordBuffer(6,t);
        
        m_Cursor    = new UIElement();
        m_UIManager->AddElement(m_Cursor);
        
        m_CursorObj = m_Renderer ->CreateRenderObject(ROT_MESH);
        m_CursorMat = m_Renderer ->CreateMaterial();
        m_CursorMat->SetMap(Material::MT_DIFFUSE,m_Textures[m_CursorTexIndex]);
        m_Textures[m_CursorTexIndex]->Destroy();
        
        m_CursorMat->SetShader   (m_UIManager->GetDefaultTextureShader());
        m_CursorObj->SetMesh     (m,m_CursorMat);
        m_Cursor   ->SetObject   (m_CursorObj);
        m_Cursor   ->SetSize     (s,s);
        
        m_UIElements.push_back(m_Cursor   );
        m_Meshes    .push_back(m_CursorObj);
        m_Materials .push_back(m_CursorMat);
    }
    void Test::InitFlyCamera(const Vec3& InitPos)
    {
        m_CamPos = InitPos;
        m_xCamRot = Quat(0,1,0,0);
        m_yCamRot = Quat(1,0,0,0);
        m_CamRot  = Quat(0,1,0,0);
        m_FlyCameraEnabled = true;
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
        RenderObject* LObj = m_Renderer->CreateRenderObject(ROT_LIGHT);
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
            Obj = m_Renderer->CreateRenderObject(ROT_MESH);
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
                
                Tex->SetPixel(Vec2(x,y),C);
            }
        }
        Tex->UpdateTexture();
        m_Textures.push_back(Tex);
        
        printf("Tex: 0x%lX <%s>\n",(intptr_t)Tex,Path);
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

        Shader* Shdr = m_ShaderGenerator->Generate();
        Mat->SetShader(Shdr);
        Shdr->Destroy();
        
        Mat->SetSpecular(Vec4(0,0,0,0));
        Mat->SetShininess(0.0f);
        
        m_Materials.push_back(Mat);
        return Mat;
    }
    
    f64 ConvertUnit(f64 n)
    {
             if(n >= 1000.0          && n < 1000000.0         ) return n * 0.001         ;
        else if(n >= 1000000.0       && n < 1000000000.0      ) return n * 0.000001      ;
        else if(n >= 1000000000.0    && n < 1000000000000.0   ) return n * 0.000000001   ;
        else if(n >= 1000000000000.0 && n < 1000000000000000.0) return n * 0.000000000001;
        return n * 1.0;
    }
    string GetPrefix(f64 n)
    {
             if(n >= 1000.0          && n < 1000000.0         ) return "k";
        else if(n >= 1000000.0       && n < 1000000000.0      ) return "M";
        else if(n >= 1000000000.0    && n < 1000000000000.0   ) return "G";
        else if(n >= 1000000000000.0 && n < 1000000000000000.0) return "T";
        return " ";
    }
    void Test::LimitFPS()
    {
        Vec3 a(3 ,21,39);
        Vec3 b(10,4 ,31);
        Vec3 u(0 ,1 , 0);
        f32  c = 10;
        
        m_FLOPSPerFrame  = 0.0f;
        m_FLOPSPerSecond = 0.0f;
        
        Timer flopTmr;
        flopTmr.Start();
        f32 tdt = 1.0f / m_TargetFrameRate;
        while(m_DeltaTime + flopTmr < tdt)
        {
            for(f32 i = 0;i < c;i++) (b - a).Normalized().Dot(u);
            m_FLOPSPerFrame += 22.0f * c;
            m_FLOPSPerFrame += 12.0f;
        }
        m_DeltaTime += flopTmr;
        m_FLOPSPerSecond = m_FLOPSPerFrame / flopTmr;
        m_FreeFLOPSSamples     .SetSampleCount(m_FramePrintInterval / m_DeltaTime);
        m_FLOPSPerSecondSamples.SetSampleCount(m_FramePrintInterval / m_DeltaTime);
        m_FreeFLOPSSamples     .AddSample(m_FLOPSPerFrame );
        m_FLOPSPerSecondSamples.AddSample(m_FLOPSPerSecond);
    }
    void Test::PrintDebugInfo()
    {
        const Renderer::RenderStats& Stats = m_Renderer->GetRenderStatistics();
        f32 vc    = Stats.VertexCount   ;
        f32 tc    = Stats.TriangleCount ;
        f32 dc    = Stats.DrawCalls     ;
        f32 fr    = Stats.FrameRate     ;
        f32 vo    = Stats.VisibleObjects;
        f32 fls   = m_FLOPSPerSecond    ;
        f32 fl    = m_FLOPSPerFrame     ;
        f32 ceff  = (Stats.MultithreadedCullingEfficiency - 1.0f) * 100.0f;
        
        f32 avc    = Stats.AverageVertexCount   .GetAverage();
        f32 atc    = Stats.AverageTriangleCount .GetAverage();
        f32 adc    = Stats.AverageDrawCalls     .GetAverage();
        f32 afr    = Stats.AverageFramerate     .GetAverage();
        f32 avo    = Stats.AverageVisibleObjects.GetAverage();
        f32 afls   = m_FLOPSPerSecondSamples    .GetAverage();
        f32 afl    = m_FreeFLOPSSamples         .GetAverage();
        f32 aceff  = (Stats.AverageMultithreadedCullingEfficiency.GetAverage() - 1.0f) * 100.0f;
        if(ceff  < 0.0f) ceff = 0.0f;
        if(aceff < 0.0f) aceff = 0.0f;
        
        printf("+--------------(Render Statistics)--------------+\n");
        printf("| Frame ID    : %10lld f  " "                  |\n",Stats.FrameID);
        printf("| Run time    : %10.3f s  " "                  |\n",m_ElapsedTime);
        
                           printf("| Frame Rate  : %8.3f %sHz "     " (A: %7.2f" " %sHz )"   " |\n",     ConvertUnit(fr  ),GetPrefix(fr  ).c_str(),ConvertUnit(afr  ),GetPrefix(afr  ).c_str());
        if(dc  >= 1000.0f) printf("| Draw calls  : %8.3f %s   "     " (A: %7.2f" " %s   )"   " |\n",     ConvertUnit(dc  ),GetPrefix(dc  ).c_str(),ConvertUnit(adc  ),GetPrefix(adc  ).c_str());
        else               printf("| Draw calls  : %8d     "        " (A: %7.2f" " %s   )"   " |\n",(i32)ConvertUnit(dc  )                        ,ConvertUnit(adc  ),GetPrefix(adc  ).c_str());
        if(vc  >= 1000.0f) printf("| Vertices    : %8.3f %s   "     " (A: %7.2f" " %s   )"   " |\n",     ConvertUnit(vc  ),GetPrefix(vc  ).c_str(),ConvertUnit(avc  ),GetPrefix(avc  ).c_str());
        else               printf("| Vertices    : %8d     "        " (A: %7.2f" " %s   )"   " |\n",(i32)ConvertUnit(vc  )                        ,ConvertUnit(avc  ),GetPrefix(avc  ).c_str());
        if(tc  >= 1000.0f) printf("| Triangles   : %8.3f %s   "     " (A: %7.2f" " %s   )"   " |\n",     ConvertUnit(tc  ),GetPrefix(tc  ).c_str(),ConvertUnit(atc  ),GetPrefix(atc  ).c_str());
        else               printf("| Triangles   : %8d     "        " (A: %7.2f" " %s   )"   " |\n",(i32)ConvertUnit(tc  )                        ,ConvertUnit(atc  ),GetPrefix(atc  ).c_str());
        if(vo  >= 1000.0f) printf("| Object Count: %8.3f %s   "     " (A: %7.2f" " %s   )"   " |\n",     ConvertUnit(vo  ),GetPrefix(vo  ).c_str(),ConvertUnit(avo  ),GetPrefix(avo  ).c_str());
        else               printf("| Object Count: %8d     "        " (A: %7.2f" " %s   )"   " |\n",(i32)ConvertUnit(vo  )                        ,ConvertUnit(avo  ),GetPrefix(avo  ).c_str());
        if(fls >= 1000.0f) printf("| FLOPS/second: %8.3f %so/s"     " (A: %7.2f" " %so/s)"   " |\n",     ConvertUnit(fls ),GetPrefix(fls ).c_str(),ConvertUnit(afls ),GetPrefix(afls ).c_str());
        else               printf("| FLOPS/second: %8d  o/s"        " (A: %7.2f" " %so/s)"   " |\n",(i32)ConvertUnit(fls )                        ,ConvertUnit(afls ),GetPrefix(afls ).c_str());
        if(fl  >= 1000.0f) printf("| Free Flt Ops: %8.3f %so/f"     " (A: %7.2f" " %so/f)"   " |\n",     ConvertUnit(fl  ),GetPrefix(fl  ).c_str(),ConvertUnit(afl  ),GetPrefix(afl  ).c_str());
        else               printf("| Free Flt Ops: %8d  o/f"        " (A: %7.2f" " %so/f)"   " |\n",(i32)ConvertUnit(fl  )                        ,ConvertUnit(afl  ),GetPrefix(afl  ).c_str());
                           printf("| Cull Effic. : %8.3f %s \%% "   " (A: %7.2f" " %s \%% )" " |\n",     ConvertUnit(ceff),GetPrefix(ceff).c_str(),ConvertUnit(aceff),GetPrefix(aceff).c_str());
        printf("+----------(Sample Duration: %6.2f s)----------+\n",m_Renderer->GetAverageSampleDuration());
    }
    bool Test::IsRunning()
    {
        if(m_UIManager)
        {
            if(m_UIManager->IsButtonDown(BTN_QUIT)) m_DoShutdown = true;
        }
        if(m_Renderer->GetRenderStatistics().FrameID > 0)
        {
            /* End previous frame */
            m_Window->SwapBuffers();
            m_TaskManager->EndFrame();
            
            /* Time stuff */
            m_LastElapsedTime = m_ElapsedTime;
            m_ElapsedTime     = m_Window->GetElapsedTime();
            m_DeltaTime       = m_ElapsedTime - m_LastElapsedTime;
            
            /* Limit framerate and measure extra time in float operations */
            LimitFPS();
        }
        
        /* Start new frame */
        if(!m_Window->GetCloseRequested() && !m_DoShutdown)
        {
            if(m_UIManager) m_UIManager->ResetCursorDelta();
            
            /* Prepare the task manager */
            m_TaskManager->BeginFrame();
            
            /* Check inputs */
            m_Window->PollEvents();
            
            /* Clear screen */
            m_Rasterizer->ClearActiveFramebuffer();
            
            /* Adjust aspect ratio (in case window resized) */
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            m_Camera->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
            
            /* Transform camera */
            if(m_FlyCameraEnabled && m_UIManager)
            {
                Vec2 CursorDelta = m_UIManager->GetUnBoundedCursorDelta() * 0.5f;
                if(CursorDelta.Magnitude() > 0.01f)
                {
                    m_xCamRot *= Quat(Vec3(0,1,0), CursorDelta.x * CAMERA_TURN_SPEED);
                    m_yCamRot *= Quat(Vec3(1,0,0),-CursorDelta.y * CAMERA_TURN_SPEED);
                    m_CamRot = m_xCamRot * m_yCamRot;
                }
                if(m_UIManager->IsButtonDown(BTN_MOVE_FORWARD )) m_CamPos += m_CamRot * Vec3(0,0,-CAMERA_MOVE_SPEED) * GetDeltaTime();
                if(m_UIManager->IsButtonDown(BTN_MOVE_BACKWARD)) m_CamPos += m_CamRot * Vec3(0,0, CAMERA_MOVE_SPEED) * GetDeltaTime();
                if(m_UIManager->IsButtonDown(BTN_MOVE_LEFT    )) m_CamPos += m_CamRot * Vec3(-CAMERA_MOVE_SPEED,0,0) * GetDeltaTime();
                if(m_UIManager->IsButtonDown(BTN_MOVE_RIGHT   )) m_CamPos += m_CamRot * Vec3( CAMERA_MOVE_SPEED,0,0) * GetDeltaTime();
                
                m_Camera->SetTransform(Translation(m_CamPos) * m_CamRot.ToMat());
            }
            
            /* Compute ray from cursor into world (could be useful) */
            f64 x,y;
            glfwGetCursorPos(m_Window->GetWindow(),&x,&y);
            GLint Viewport[4];
            glGetIntegerv(GL_VIEWPORT,Viewport);
            Mat4 v = m_Camera->GetTransform();
            Mat4 p = m_Camera->GetProjection();
            m_CursorRay = UnProject(Vec3(x,y,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
            
            /* Update cursor position */
            if(m_UIManager && m_CursorObj)
            {
                Vec2 cPos = m_UIManager->GetCursorPosition();
                m_CursorObj->SetTransform(Translation(Vec3(cPos.x,cPos.y,-99.0f)));
            }
            
            /* Update UI */
            if(m_UIManager) m_UIManager->Update(m_DeltaTime);
            
            /* Render */
            m_Renderer->Render(m_DeltaTime,PT_TRIANGLES);
            
            m_FramePrintTime += m_DeltaTime;
            if(m_FramePrintTime > m_FramePrintInterval)
            {
                PrintDebugInfo();
                m_FramePrintTime  = 0.0f;
            }
            return true;
        }
        return false;
    }
    Vec2 Test::GetPreferredInitResolution() const
    {
        #ifdef __APPLE__
        return Vec2(400,300);
        #else
        return Vec2(800,600);
        #endif
    }
};