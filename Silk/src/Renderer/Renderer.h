#pragma once
#include <Renderer/RenderObject.h>
#include <Renderer/Camera.h>
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Renderer/Scene.h>
#include <Renderer/Texture.h>
#include <Renderer/ShaderSystem.h>
#include <Renderer/UniformBufferTypes.h>
#include <Renderer/CullingAlgorithms/NullCullingAlgorithm.h>
#include <Renderer/CullingAlgorithms/BruteForceCullingAlgorithm.h>

#include <System/TaskManager.h>
#include <Utilities/Utilities.h>
#include <Utilities/DebugDrawer.h>

namespace Silk
{
    class UIManager;
    class UniformBuffer;
    class Renderer
    {
        public:
			struct RenderPreferences
            {
                /* Per object */
                i32 MaxLights;
                
                /* Seconds */
				f32 AverageSampleDuration;
                
                /* Self explanatory */
                i32 MinObjectCountForMultithreadedCulling;
                i32 MinObjectCountForMultithreadedTransformSync;
            };

            struct RenderStats
            {
                i64 FrameID;
                
                i32 DrawCalls;
                SampleBuffer AverageDrawCalls;
                
                i32 VertexCount;
                SampleBuffer AverageVertexCount;
                
                i32 TriangleCount;
                SampleBuffer AverageTriangleCount;
                
				i32 VisibleObjects;
                SampleBuffer AverageVisibleObjects;
                
                f32 FrameRate;
                SampleBuffer AverageFramerate;
                
                f32 MultithreadedCullingEfficiency;
                SampleBuffer AverageMultithreadedCullingEfficiency;
            };

            Renderer(Rasterizer* Raster,TaskManager* TaskMgr);
            virtual ~Renderer();
        
            void SetUIManager(UIManager* Mgr) { m_UIManager = Mgr; }
            UIManager* GetUIManager() const { return m_UIManager; }
            void SetDebugDrawer(DebugDrawer* Draw) { m_DebugDrawer = Draw; }
            DebugDrawer* GetDebugDrawer() const { return m_DebugDrawer; }
        
            void Init();
            
            Rasterizer     * GetRasterizer           () { return m_Raster         ; }
            TaskManager    * GetTaskManager          () { return m_TaskManager    ; }
            ShaderGenerator* GetShaderGenerator      () { return m_ShaderGenerator; }
            UIManager      * GetUIManager            () { return m_UIManager      ; }
            DebugDrawer    * GetDebugDrawer          () { return m_DebugDrawer    ; }
            Texture        * GetDefaultTexture       ();
            UniformBuffer  * GetEngineUniformBuffer  () { return m_EngineUniforms                 ; }
            UniformBuffer  * GetRendererUniformBuffer() { return m_RendererUniforms->GetUniforms(); }
        
            RenderObject* CreateRenderObject(RENDER_OBJECT_TYPE Rot);
            Material    * CreateMaterial();
        
            /* Call these only from the ShaderGenerator and Shader classes, respectively */
            void RequireFragmentOutput(ShaderGenerator::OUTPUT_FRAGMENT_TYPE Type);
            void ReleaseFragmentOutput(ShaderGenerator::OUTPUT_FRAGMENT_TYPE Type);
        
            void Destroy(Material    * Mat);
            void Destroy(RenderObject* Obj);
        
            void UpdateUniforms();
            void Render(Scalar dt,PRIMITIVE_TYPE PrimType);
            void RenderObjects(ObjectList* List,PRIMITIVE_TYPE PrimType);
            void RenderTexture(Texture* Tex,Material* Effect = 0);
        
            void ClearScene() { if(m_Scene) { delete m_Scene; } m_Scene = new Scene(this); }
            Scene* GetScene() const { return m_Scene; }

			/*
			 * Render preferences
			 */
        

            //Max lights per object
			void SetMaxLights(i32 MaxLights) { m_Prefs.MaxLights = MaxLights; m_DoRecompileAllShaders = true; }
            i32 GetMaxLights() { return m_Prefs.MaxLights; }
			
			//Sample duration (seconds) for averages of render statistics
			void SetAverageSampleDuration(Time Duration) { m_Prefs.AverageSampleDuration = Duration; }
			Time GetAverageSampleDuration() const { return m_Prefs.AverageSampleDuration; }
        
            //Object count required before tasking is enabled for culling
            void SetMinObjectCountForMultithreadedCulling(i32 Count) { m_Prefs.MinObjectCountForMultithreadedCulling = Count; }
            i32 GetMinObjectCountForMultithreadedCulling() const { return m_Prefs.MinObjectCountForMultithreadedCulling; }
        
            //Object count required before tasking is enabled for instance transform syncing
            void SetMinObjectCountForMultithreadedTransformSync(i32 Count) { m_Prefs.MinObjectCountForMultithreadedTransformSync = Count; }
            i32 GetMinObjectCountForMultithreadedTransformSync() const { return m_Prefs.MinObjectCountForMultithreadedTransformSync; }

            /*
             * Renderer information
             */

			const RenderStats& GetRenderStatistics() const { return m_Stats; }
            Configuration* GetConfiguration() const { return m_Configuration; }
        
        protected:
            friend class UIManager;
            RenderPreferences m_Prefs;
            RenderStats m_Stats;
            bool m_DoRecompileAllShaders;
        
            bool m_DefaultTextureNeedsUpdate;
            Texture* m_DefaultTexture;
            Scalar m_DefaultTexturePhase;
            void UpdateDefaultTexture();
        
            //For rendering textures to the screen
            Material* m_DefaultFSQMaterial;
            RenderObject* m_FSQ;
        
            //For determining framebuffer attachments
            i32 m_UsedFragmentOutputs[ShaderGenerator::OFT_COUNT];
            //vector<UserFragmentOutput*> m_UserFragmentOutputs;
        
            TaskManager     * m_TaskManager     ;
            UIManager       * m_UIManager       ;
            DebugDrawer     * m_DebugDrawer     ;
            ShaderGenerator * m_ShaderGenerator ;
            Scene           * m_Scene           ;
            UniformBuffer   * m_EngineUniforms  ;
            RenderUniformSet* m_RendererUniforms;
            Rasterizer      * m_Raster          ;
            Configuration   * m_Configuration   ;
    };
};
