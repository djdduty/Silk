#pragma once
#include <Silk.h>
#include <System/TaskManager.h>
#include <ObjLoader.h>
#include <Window.h>
#include <InputManager.h>
#include <Renderer/Renderer.h>

#define CAMERA_MOVE_SPEED 15.0f
#define CAMERA_TURN_SPEED 0.6f

namespace TestClient
{
    enum BUTTON_ID
    {
        BTN_LEFT_MOUSE   ,
        BTN_RIGHT_MOUSE  ,
        
        BTN_MOVE_FORWARD ,  //W
        BTN_MOVE_BACKWARD,  //S
        BTN_MOVE_LEFT    ,  //A
        BTN_MOVE_RIGHT   ,  //D
        
        BTN_UP_ARROW     ,
        BTN_DOWN_ARROW   ,
        BTN_LEFT_ARROW   ,
        BTN_RIGHT_ARROW  ,
        
        BTN_TOGGLE       ,
        BTN_TOGGLE_STATS ,
        
        BTN_P            ,
		BTN_L			 ,
        
        BTN_QUIT         ,  //Escape
        
        BTN_COUNT,
    };
    
    class Test
    {
        public:
            Test();
            virtual ~Test();
        
            void Init();
            void InitGUI();
			UIText* CreateRenderText(Vec3 Position, std::string Text, UIPanel* Panel);

			void InitRenderGUI();
			void InitDebugDisplay();
            void InitFlyCamera(const Vec3& InitPos = Vec3(0,0,0));
            void InitSSAO();
            void SetSSAORadius(Scalar Radius);
            void SetSSAOIntensity(Scalar Intensity);
            void SetSSAONoiseScale(i32 Scale);
        
            UIManager   * GetUI    () const { return m_UIManager   ; }
            InputManager* GetInput () const { return m_InputManager; }
            Camera      * GetCamera() const { return m_Camera      ; }
        
            Byte* Load(const char* File,i64 *OutSize = 0);
            bool  Save(const char* File,Byte* Data,i64 Size);
        
            RenderObject* AddLight(LightType Type,const Vec3& Pos);
            i32 AddMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count = 1);
            RenderObject* LoadMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count = 1) { return m_Meshes[AddMesh(Path,Mat,Pos,Count)]; }
        
            Texture * LoadTexture(const char *Path);
            Material* AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse = 0,const char* Normal = 0,const char* Parallax = 0);
            Material* AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,Texture* Diffuse,Texture* Normal,Texture* Parallax);
        
            void SetFPSPrintFrequency(Scalar Hz) { m_FramePrintInterval = 1.0f / Hz; }
            void SetTargetFrameRate  (Scalar Hz) { m_TargetFrameRate = Hz; m_TaskManager->GetTaskContainer()->SetTimestep(Hz); }
        
            void LimitFPS ();
            void PrintDebugInfo();
            bool IsRunning();
            Scalar GetDeltaTime() { return m_DeltaTime; }
        
            virtual void Run() = 0;
            virtual void Shutdown() = 0;
            virtual const char* GetTestName() const = 0;
            virtual Vec2 GetPreferredInitResolution() const;
			virtual CullingAlgorithm* GetPreferredCullingAlgorithm() const { return new BruteForceCullingAlgorithm(m_Renderer->GetScene(),m_TaskManager); }
            virtual Renderer* GetPreferredRenderer(Rasterizer* Raster, TaskManager* TaskMng) const { return new Renderer(Raster,TaskMng); }
        
            Renderer* GetRenderer() const { return m_Renderer; }
            Rasterizer* GetRasterizer() const { return m_Rasterizer; }
        
            Window         * m_Window;
            Renderer       * m_Renderer;
            Rasterizer     * m_Rasterizer;
            ShaderGenerator* m_ShaderGenerator;
            DebugDrawer    * m_DebugDraw;
        
            Camera         * m_Camera;
        
        protected:
            virtual void Initialize() = 0;
            TaskManager * m_TaskManager ;
            UIManager   * m_UIManager   ;
            InputManager* m_InputManager;
        
            Scalar m_ElapsedTime        ;
            Scalar m_LastElapsedTime    ;
            Scalar m_DeltaTime          ;
            Scalar m_TargetFrameRate    ;
            Scalar m_FramePrintTime     ;
            Scalar m_FramePrintInterval ;
            Scalar m_FLOPSPerSecond     ;
            Scalar m_FLOPSPerFrame      ;
            SampleBuffer m_FreeFLOPSSamples     ;
            SampleBuffer m_FLOPSPerSecondSamples;
            bool m_DoShutdown;
            
            Vec3 m_CamPos;
            Quat m_xCamRot;
            Quat m_yCamRot;
            Quat m_CamRot;
            Scalar m_CamSpeed;
            bool m_FlyCameraEnabled;
        
            UIElement   * m_Cursor;
            i32 m_CursorTexIndex;
            Ray m_CursorRay;

            Scalar m_StatsTranslation;
            Scalar m_StatsVelocity;
            bool m_StatsButtonHeld;
            UIElement* m_StatsPanel;
			UIText* m_UIFrameID;
			UIText* m_UIRunTime;
			UIText* m_UIFrameRate;
			UIText* m_UIDrawCalls;
			UIText* m_UIVertices;
			UIText* m_UITriangles;
			UIText* m_UIObjectcount;
			bool m_UsingRenderUI;
        
            i32 m_RTTIndex;
        
            PostProcessingEffect* m_SSAO;
            Texture* m_SSAORandomTex;
        
            ObjLoader* m_ObjLoader;
            vector<UIElement   *> m_UIElements ;
            vector<RenderObject*> m_Meshes     ;
            vector<Material    *> m_Materials  ;
            vector<Texture     *> m_Textures   ;
            vector<RenderObject*> m_Lights     ;
            vector<RenderObject*> m_LightMeshes;
    };
};
