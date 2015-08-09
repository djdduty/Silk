#pragma once
#include <Silk.h>
#include <System/TaskManager.h>
#include <ObjLoader.h>
#include <Window.h>

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
        
        BTN_COUNT,
    };
    
    class Test
    {
        public:
            Test();
            virtual ~Test();
        
            void Init();
            void InitGUI();
            void InitCursor();
        
            UIManager* GetUI() const { return m_UIManager; }
        
            Byte* Load(const char* File,i64 *OutSize = 0);
            bool  Save(const char* File,Byte* Data,i64 Size);
        
            RenderObject* AddLight(LightType Type,const Vec3& Pos);
            i32 AddMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count = 1);
            Texture* LoadTexture(const char *Path);
            Material* AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse = 0,const char* Normal = 0,const char* Parallax = 0);
        
            void SetFPSPrintFrequency(Scalar Hz) { m_FramePrintInterval = 1.0f / Hz; }
            bool IsRunning();
            Scalar GetDeltaTime() { return m_DeltaTime; }
        
            virtual void Run() = 0;
            virtual void Shutdown() = 0;
            virtual const char* GetTestName() const = 0;
        
            Window         * m_Window;
            Renderer       * m_Renderer;
            Rasterizer     * m_Rasterizer;
            ShaderGenerator* m_ShaderGenerator;
        
            Camera         * m_Camera;
        
        protected:
            virtual void Initialize() = 0;
            TaskManager* m_TaskManager;
            UIManager  * m_UIManager;
            Scalar m_ElapsedTime;
            Scalar m_LastElapsedTime;
            Scalar m_DeltaTime;
            Scalar m_FramePrintTime;
            Scalar m_FramePrintInterval;
            bool m_DoShutdown;
        
            UIElement   * m_Cursor;
            RenderObject* m_CursorObj;
            Material    * m_CursorMat;
            i32 m_CursorTexIndex;
            Ray m_CursorRay;
        
            ObjLoader* m_ObjLoader;
            vector<RenderObject*> m_Meshes     ;
            vector<Material    *> m_Materials  ;
            vector<Texture     *> m_Textures   ;
            vector<RenderObject*> m_Lights     ;
            vector<RenderObject*> m_LightMeshes;
    };
};