#include <Test.h>
#include <Window.h>
#include <LodePNG.h>
#include <sstream>
#include <iomanip>

namespace TestClient
{
    static Test* g_Test = 0;
    void OnCursorMove(GLFWwindow* Win,f64 x,f64 y)
    {
        if(!g_Test) return;
        g_Test->GetInput()->SetCursorPosition(Vec2(x,y));
    }
    void OnClick(GLFWwindow* Win,i32 Button,i32 Action,i32 Mods)
    {
        if(!g_Test) return;
        
        if(Button == GLFW_MOUSE_BUTTON_LEFT)
        {
                 if(Action == GLFW_PRESS  ) g_Test->GetInput()->OnButtonDown(BTN_LEFT_MOUSE);
            else if(Action == GLFW_RELEASE) g_Test->GetInput()->OnButtonUp  (BTN_LEFT_MOUSE);
        }
        else if(Button == GLFW_MOUSE_BUTTON_RIGHT)
        {
                 if(Action == GLFW_PRESS  ) g_Test->GetInput()->OnButtonDown(BTN_RIGHT_MOUSE);
            else if(Action == GLFW_RELEASE) g_Test->GetInput()->OnButtonUp  (BTN_RIGHT_MOUSE);
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
                 if(Key == GLFW_KEY_W     ) g_Test->GetInput()->OnButtonDown(BTN_MOVE_FORWARD );
            else if(Key == GLFW_KEY_S     ) g_Test->GetInput()->OnButtonDown(BTN_MOVE_BACKWARD);
            else if(Key == GLFW_KEY_A     ) g_Test->GetInput()->OnButtonDown(BTN_MOVE_LEFT    );
            else if(Key == GLFW_KEY_D     ) g_Test->GetInput()->OnButtonDown(BTN_MOVE_RIGHT   );
            else if(Key == GLFW_KEY_ESCAPE) g_Test->GetInput()->OnButtonDown(BTN_QUIT         );
            else if(Key == GLFW_KEY_UP    ) g_Test->GetInput()->OnButtonDown(BTN_UP_ARROW     );
            else if(Key == GLFW_KEY_DOWN  ) g_Test->GetInput()->OnButtonDown(BTN_DOWN_ARROW   );
            else if(Key == GLFW_KEY_LEFT  ) g_Test->GetInput()->OnButtonDown(BTN_LEFT_ARROW   );
            else if(Key == GLFW_KEY_RIGHT ) g_Test->GetInput()->OnButtonDown(BTN_RIGHT_ARROW  );
            else if(Key == GLFW_KEY_I     ) g_Test->GetInput()->OnButtonDown(BTN_TOGGLE_STATS );
            else if(Key == GLFW_KEY_P     ) g_Test->GetInput()->OnButtonDown(BTN_P            );
            else if(Key == GLFW_KEY_L     ) g_Test->GetInput()->OnButtonDown(BTN_L            );
            else if(Key == GLFW_KEY_WORLD_1
		        ||  Key == GLFW_KEY_GRAVE_ACCENT) g_Test->GetInput()->OnButtonDown(BTN_TOGGLE );
        }
        else if(Action == GLFW_RELEASE)
        {
                 if(Key == GLFW_KEY_W     ) g_Test->GetInput()->OnButtonUp(BTN_MOVE_FORWARD );
            else if(Key == GLFW_KEY_S     ) g_Test->GetInput()->OnButtonUp(BTN_MOVE_BACKWARD);
            else if(Key == GLFW_KEY_A     ) g_Test->GetInput()->OnButtonUp(BTN_MOVE_LEFT    );
            else if(Key == GLFW_KEY_D     ) g_Test->GetInput()->OnButtonUp(BTN_MOVE_RIGHT   );
            else if(Key == GLFW_KEY_ESCAPE) g_Test->GetInput()->OnButtonUp(BTN_QUIT         );
            else if(Key == GLFW_KEY_UP    ) g_Test->GetInput()->OnButtonUp(BTN_UP_ARROW     );
            else if(Key == GLFW_KEY_DOWN  ) g_Test->GetInput()->OnButtonUp(BTN_DOWN_ARROW   );
            else if(Key == GLFW_KEY_LEFT  ) g_Test->GetInput()->OnButtonUp(BTN_LEFT_ARROW   );
            else if(Key == GLFW_KEY_RIGHT ) g_Test->GetInput()->OnButtonUp(BTN_RIGHT_ARROW  );
            else if(Key == GLFW_KEY_I     ) g_Test->GetInput()->OnButtonUp(BTN_TOGGLE_STATS );
            else if(Key == GLFW_KEY_P     ) g_Test->GetInput()->OnButtonUp(BTN_P            );
            else if(Key == GLFW_KEY_L     ) g_Test->GetInput()->OnButtonUp(BTN_L            );
            else if(Key == GLFW_KEY_WORLD_1
		        ||  Key == GLFW_KEY_GRAVE_ACCENT) g_Test->GetInput()->OnButtonUp(BTN_TOGGLE );
        }
    }
    
    Test::Test() : m_UsingRenderUI(false)
    {
    }
    Test::~Test()
    {
        exit(0);
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
            m_Renderer = GetPreferredRenderer(m_Rasterizer,m_TaskManager);
            m_Rasterizer->SetRenderer(m_Renderer);
            m_Renderer->Init();
            
            m_InputManager = new InputManager(m_Renderer);
            m_InputManager->Initialize(BTN_COUNT);
            m_InputManager->SetMouseButtonIDs(BTN_LEFT_MOUSE,BTN_RIGHT_MOUSE);
        
            m_ShaderGenerator = m_Renderer->GetShaderGenerator();
            
            Scalar Aspect = m_Rasterizer->GetContext()->GetResolution().y / m_Rasterizer->GetContext()->GetResolution().x;
            
            m_Camera = new Camera(Vec2(60.0f,60.0f * Aspect),0.001f,2000.0f);
            m_Renderer->GetScene()->SetActiveCamera(m_Camera);
			m_Renderer->GetScene()->SetCullingAlgorithm(GetPreferredCullingAlgorithm());
            
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
            
            m_RTTIndex = -1;
            
            m_SSAO = 0;
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
        m_UIManager->Initialize();
        m_UIManager->SetZClipPlanes(0.0f,200.0f);

		Byte* fDat = Load("Common/font24px/Font.fnt");
        Font* Fnt = new Font();
        Fnt->Load(fDat);
        Fnt->SetFontImage(LoadTexture("Common/font24px/Font.png"));
        m_UIManager->SetFont(Fnt);
        
        UIPanel* p = new UIPanel(Vec2(20,20));
        p->SetBackgroundColor(Vec4(1,1,1,1));
        p->SetBackgroundImage(LoadTexture("Common/Cursor.png"));
        m_Cursor = p;
        m_UIManager->AddElement(m_Cursor);
        
        glfwSetInputMode          (m_Window->GetWindow(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        glfwSetMouseButtonCallback(m_Window->GetWindow(),OnClick     );
        glfwSetCursorPosCallback  (m_Window->GetWindow(),OnCursorMove);
        glfwSetKeyCallback        (m_Window->GetWindow(),OnKey       );
    }
	UIText* Test::CreateRenderText(Vec3 Pos, std::string text, UIPanel* Pan)
	{
		if(!m_UIManager)
			return 0;
		UIText* Tex = new UIText();
		Tex->SetFont(m_UIManager->GetFont());
		Tex->SetText(text);
		Tex->SetTextSize(22);
		Tex->SetPosition(Pos);
		Pan->AddChild(Tex);
		m_UIElements.push_back(Tex);
		return Tex;
	}
	void Test::InitRenderGUI()
	{
		if(m_UsingRenderUI)
			return;

		UIPanel* RPanel = new UIPanel(Vec2(350,168));
        m_UIManager->AddElement(RPanel);
        RPanel->SetBackgroundColor(Vec4(0,0,0,0.75));
        RPanel->SetPosition(Vec3(100,100,0));
        m_UIElements.push_back(RPanel);
        m_StatsPanel = RPanel;
        
		m_UIFrameID     = CreateRenderText(Vec3(0,0  ,0), "Frame ID:"    , RPanel);
		m_UIRunTime     = CreateRenderText(Vec3(0,24 ,0), "Run Time:"    , RPanel);
		m_UIFrameRate   = CreateRenderText(Vec3(0,48 ,0), "Frame Rate:"  , RPanel);
		m_UIDrawCalls   = CreateRenderText(Vec3(0,72 ,0), "# Draw Calls:", RPanel);
		m_UIVertices    = CreateRenderText(Vec3(0,96 ,0), "# Vertices:"  , RPanel);
		m_UITriangles   = CreateRenderText(Vec3(0,120,0), "# Triangles:" , RPanel);
		m_UIObjectcount = CreateRenderText(Vec3(0,144,0), "# Objects:"   , RPanel);

        m_StatsVelocity    = -1.0f;
        m_StatsTranslation = 0.0f;
		m_UsingRenderUI = true;
	}
    void Test::InitFlyCamera(const Vec3& InitPos)
    {
        m_CamPos = InitPos;
        m_xCamRot = Quat(0,1,0,0);
        m_yCamRot = Quat(1,0,0,0);
        m_CamRot  = Quat(0,1,0,0);
        m_CamSpeed = CAMERA_MOVE_SPEED;
        m_FlyCameraEnabled = true;
    }
	void Test::InitDebugDisplay()
	{
        m_DebugDraw = new DebugDrawer(m_Renderer);
        m_Renderer->SetDebugDrawer(m_DebugDraw);
	}
    void Test::InitSSAO()
    {
        m_Renderer->SetUsePostProcessing(true);
        
        m_SSAO = new PostProcessingEffect(m_Renderer);
        m_SSAO->LoadEffect(Load("Common/SSAO.ppe"));
        m_Renderer->AddPostProcessingEffect(m_SSAO);
        
        m_SSAORandomTex = m_Renderer->GetRasterizer()->CreateTexture();
        m_SSAORandomTex->CreateTexture(4,4,Texture::PT_FLOAT);
        
        for(i32 x = 0;x < 4;x++)
        {
            for(i32 y = 0;y < 4;y++)
            {
                m_SSAORandomTex->SetPixel(Vec2(x,y),Vec4(Random(-1.0f,1.0f),Random(-1.0f,1.0f),0.0f,1.0f));
            }
        }
        m_SSAORandomTex->UpdateTexture();
        m_SSAO->GetStage(0)->GetMaterial()->SetMap(Material::MT_CUSTOM0,m_SSAORandomTex);
        
        UniformBuffer* SSAOInputs = m_SSAO->GetStage(0)->GetMaterial()->GetUserUniforms();
        
        vector<Vec3> SSAOKernel;
        for(i32 i = 0;i < SSAOInputs->GetUniformInfo(0)->ArraySize;i++)
        {
            Vec3 Sample = RandomVec(1.0f);
            Sample.z = abs(Sample.z);
            Sample.Normalize();
            
            Scalar Scale = Scalar(i) / Scalar(SSAOInputs->GetUniformInfo(0)->ArraySize);
            Scale = 0.1f + ((1.0f - 0.1f) * pow(Scale,2.0f));
            Sample *= Scale;
            SSAOKernel.push_back(Sample);
        }
        
        SSAOInputs->SetUniform(0,SSAOKernel);
        SSAOInputs->SetUniform(1,SSAOInputs->GetUniformInfo(0)->ArraySize);
        SSAOInputs->SetUniform(2,0.2f);
        SSAOInputs->SetUniform(3,1.0f);
        SetSSAONoiseScale(4);
    }
    void Test::SetSSAORadius(Scalar Radius)
    {
        m_SSAO->GetStage(0)->GetMaterial()->GetUserUniforms()->SetUniform(2,Radius);
    }
    void Test::SetSSAOIntensity(Scalar Intensity)
    {
        m_SSAO->GetStage(0)->GetMaterial()->GetUserUniforms()->SetUniform(3,Intensity);
    }
    void Test::SetSSAONoiseScale(i32 Scale)
    {
        m_SSAO->GetStage(0)->GetMaterial()->GetUserUniforms()->SetUniform(4,Scale);
        m_SSAO->GetStage(1)->GetMaterial()->GetUserUniforms()->SetUniform(0,Scale);
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
        M->PrimitiveType = PT_TRIANGLES;
        
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
                
                Tex->SetPixel(Vec2(x,(h - 1) - y),C);
            }
        }
        Tex->UpdateTexture();
        m_Textures.push_back(Tex);
        
        printf("Tex: 0x%lX <%s>\n",(intptr_t)Tex,Path);
        return Tex;
    }
    Material* Test::AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse,const char* Normal,const char* Parallax)
    {
        Texture* D = LoadTexture(Diffuse );
        Texture* N = LoadTexture(Normal  );
        Texture* P = LoadTexture(Parallax);
        
        Material* Mat = AddMaterial(LightingMode,D,N,P);
        
        if(D) D->Destroy();
        if(N) N->Destroy();
        if(P) P->Destroy();
        
        return Mat;
    }
    Material* Test::AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,Texture* Diffuse,Texture* Normal,Texture* Parallax)
    {
        Material* Mat = m_Renderer->CreateMaterial();
        
        m_ShaderGenerator->Reset();
        
        m_ShaderGenerator->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        m_ShaderGenerator->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput(ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        
        Mat->SetMap(Material::MT_DIFFUSE ,Diffuse );
        Mat->SetMap(Material::MT_NORMAL  ,Normal  );
        Mat->SetMap(Material::MT_PARALLAX,Parallax);
        
        if(LightingMode != ShaderGenerator::LM_FLAT)
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL  ,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION,true);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TANGENT,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT,true);
            
            m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_POSITION,true);
            m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_NORMAL  ,true);
            m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
            m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_TANGENT ,true);
            m_ShaderGenerator->SetAllowInstancing(false);
            
            if(Normal || Parallax)
            {
                m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TANGENT ,true);
                m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_NORMAL  ,true);
                
                m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT ,true);
                m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
            }
            
            if(LightingMode == ShaderGenerator::LM_PASS)
            {
                m_ShaderGenerator->SetFragmentOutput(ShaderGenerator::OFT_MATERIAL0,true);
                m_ShaderGenerator->SetFragmentOutput(ShaderGenerator::OFT_MATERIAL1,true);
                m_ShaderGenerator->AddFragmentModule(const_cast<CString>(
                "[SetMaterial0]\n"
                "vec4 sMaterial0 = vec4(u_Specular,u_Shininess,0.0,0.0);\n"
                "[/SetMaterial0]\n"
                ),0);
                m_ShaderGenerator->AddFragmentModule(const_cast<CString>(
                "[SetMaterial1]\n"
                "vec4 sMaterial1 = u_Emissive;\n"
                "[/SetMaterial1]\n"
                ),1);
            }
        }
        else
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL  ,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,false);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TANGENT ,false);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT ,false);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION,false);

            m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
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
        
        m_ShaderGenerator->SetTextureInput(Material::MT_DIFFUSE ,Diffuse  != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_NORMAL  ,Normal   != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_PARALLAX,Parallax != 0);
        m_ShaderGenerator->SetLightingMode(LightingMode);

        Shader* Shdr = m_ShaderGenerator->Generate();
        Mat->SetShader(Shdr);
        Shdr->Destroy();
        
        Mat->SetSpecular(0.0f);
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
	string ftostr(f32 val)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << val;
		return ss.str();
	}
    string SecondsToTimeString(Scalar s)
    {
        i32 Hours   =  s / 60.0f / 60.0f;
        i32 Minutes = (s / 60.0f) - (Hours * 60 * 60);
        i32 Seconds = s - (Hours * 60 * 60) - (Minutes * 60);
        return FormatString("%s%d:%s%d:%s%d",Hours   <= 9 ? "0" : "",Hours,
                                             Minutes <= 9 ? "0" : "",Minutes,
                                             Seconds <= 9 ? "0" : "",Seconds);
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

		if(m_UsingRenderUI)
        {
            if(m_StatsPanel->IsEnabled())
            {
                m_UIFrameID     ->SetText("Frame ID: "       + to_string(Stats.FrameID));
                m_UIRunTime     ->SetText("Run Time: "       + SecondsToTimeString(m_ElapsedTime));
                m_UIFrameRate   ->SetText("Avg Frame Rate: " + to_string((i32)afr)+"Hz (" + ftostr((1000.0 / afr)) + "ms)");
                m_UIDrawCalls   ->SetText("Avg Draw Calls: " + to_string((i32)adc));
                m_UIVertices    ->SetText("Avg Vertices: "   + to_string((i32)avc));
                m_UITriangles   ->SetText("Avg Triangles: "  + to_string((i32)atc));
                m_UIObjectcount ->SetText("Avg Objects: "    + to_string((i32)avo));
            }
            
            if(m_InputManager->GetButtonDownDuration(BTN_TOGGLE_STATS) > 0.0f && !m_StatsButtonHeld)
            {
                m_StatsButtonHeld = true;
                if(m_StatsTranslation == 1.0f || m_StatsVelocity > 0.0f) m_StatsVelocity = -4.5f;
                else if(m_StatsTranslation == 0.0f || m_StatsVelocity < 0.0f) { m_StatsVelocity = 4.5f; m_StatsPanel->SetEnabled(true); }
            }
            else if(m_InputManager->GetButtonDownDuration(BTN_TOGGLE_STATS) == -1.0f) m_StatsButtonHeld = false;
        
            if(m_StatsVelocity != 0.0f)
            {
                m_StatsTranslation += m_StatsVelocity * GetDeltaTime();
                if(m_StatsTranslation > 1.0f)
                {
                    m_StatsVelocity = 0.0f;
                    m_StatsTranslation = 1.0f;
                }
                else if(m_StatsTranslation < 0.0f)
                {
                    m_StatsVelocity = 0.0f;
                    m_StatsTranslation = 0.0f;
                    m_StatsPanel->SetEnabled(false);
                }
                
                UIRect* r = m_StatsPanel->GetBounds();
                Vec2 res = m_Rasterizer->GetContext()->GetResolution();
                m_StatsPanel->SetPosition(Vec3(res.x - (r->GetDimensions().x * m_StatsTranslation),res.y - r->GetDimensions().y,0.0f));
            }
		} else {
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
    }
    bool Test::IsRunning()
    {
        if(m_UIManager)
        {
            if(m_InputManager->IsButtonDown(BTN_QUIT)) m_DoShutdown = true;
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
            //LimitFPS();
        }
        
        /* Start new frame */
        if(!m_Window->GetCloseRequested() && !m_DoShutdown)
        {
            if(m_UIManager) m_InputManager->ResetCursorDelta();
            
            /* Prepare the task manager */
            m_TaskManager->BeginFrame();
            
            /* Check inputs */
            m_Window->PollEvents();
            
            /* Clear screen */
            m_Rasterizer->ClearActiveFramebuffer();
            
            /* Transform camera */
            if(m_FlyCameraEnabled && m_UIManager)
            {
                Vec2 CursorDelta = m_InputManager->GetRealCursorDelta() * 0.5f;
                
                if(CursorDelta.Magnitude() > 0.01f)
                {
                    m_xCamRot *= Quat(Vec3(0,1,0),-CursorDelta.x * CAMERA_TURN_SPEED);
                    m_yCamRot *= Quat(Vec3(1,0,0),-CursorDelta.y * CAMERA_TURN_SPEED);
                    m_CamRot = m_xCamRot * m_yCamRot;
                }
                if(m_InputManager->IsButtonDown(BTN_UP_ARROW     )) m_CamSpeed += 50.0f * GetDeltaTime();
                if(m_InputManager->IsButtonDown(BTN_DOWN_ARROW   )) m_CamSpeed -= 50.0f * GetDeltaTime();
                if(m_InputManager->IsButtonDown(BTN_MOVE_FORWARD )) m_CamPos += m_CamRot * Vec3(0,0,-m_CamSpeed) * GetDeltaTime();
                if(m_InputManager->IsButtonDown(BTN_MOVE_BACKWARD)) m_CamPos += m_CamRot * Vec3(0,0, m_CamSpeed) * GetDeltaTime();
                if(m_InputManager->IsButtonDown(BTN_MOVE_LEFT    )) m_CamPos += m_CamRot * Vec3(-m_CamSpeed,0,0) * GetDeltaTime();
                if(m_InputManager->IsButtonDown(BTN_MOVE_RIGHT   )) m_CamPos += m_CamRot * Vec3( m_CamSpeed,0,0) * GetDeltaTime();
                
                m_Camera->SetTransform(Translation(m_CamPos) * m_CamRot.ToMat());
            }
            
            /* Compute ray from cursor into world (could be useful) */
            f64 x,y;
            glfwGetCursorPos(m_Window->GetWindow(),&x,&y);
            GLint Viewport[4];
            glGetIntegerv(GL_VIEWPORT,Viewport);
            Mat4 v = m_Camera->GetTransform ();
            Mat4 p = m_Camera->GetProjection();
            m_CursorRay = UnProject(Vec3(x,y,0),v,p,Vec4(Viewport[0],Viewport[1],Viewport[2],Viewport[3]));
            
            /* Adjust aspect ratio (in case window resized) */
            Scalar Aspect = Scalar(Viewport[3]) / Scalar(Viewport[2]);
            m_Camera->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
            
            /* Update cursor position */
            if(m_UIManager && m_Cursor)
            {
                Vec2 cPos = m_InputManager->GetCursorPosition();
                m_Cursor->SetPosition(Vec3(cPos.x,cPos.y,-99.0f));
            }
            
            /* Update UI */
            m_InputManager->Update(m_DeltaTime);
            
            /* Render */
            if(m_RTTIndex != -1) m_Textures[m_RTTIndex]->EnableRTT(true);
            m_Renderer->Render(m_DeltaTime,PT_TRIANGLES);
            if(m_RTTIndex != -1) m_Textures[m_RTTIndex]->DisableRTT();
            
            m_FramePrintTime += m_DeltaTime;
            if(m_FramePrintTime > m_FramePrintInterval || m_UsingRenderUI)
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
