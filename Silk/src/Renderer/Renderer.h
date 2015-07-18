#ifndef SILK_RENDERER_H
#define SILK_RENDERER_H
#include <Renderer/RenderObject.h>
#include <Renderer/Camera.h>
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Renderer/UniformBufferTypes.h>
#include <Renderer/Scene.h>

#include <Renderer/CullingAlgorithms/NullCullingAlgorithm.h>

namespace Silk
{
    class UniformBuffer;
    class Renderer
    {
        public:
            Renderer(Rasterizer* Raster);
            virtual ~Renderer();
        
            Rasterizer*    GetRasterizer           () { return m_Raster; }
            Texture*       GetDefaultTexture       ();
            UniformBuffer* GetEngineUniformBuffer  () { return m_EngineUniforms                 ; }
            UniformBuffer* GetRendererUniformBuffer() { return m_RendererUniforms->GetUniforms(); }
        
            RenderObject* CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene = true);
            Material    * CreateMaterial();
            void Destroy(Material    * Mat);
            void Destroy(RenderObject* Obj);
        
            void UpdateUniforms();
            void Render(i32 PrimType);
        
            void SetMaxLights(i32 MaxLights) { m_Prefs.MaxLights = MaxLights; m_DoRecompileAllShaders = true; }
            i32 GetMaxLights() { return m_Prefs.MaxLights; }
        
            void ClearScene() { if(m_Scene) { delete m_Scene; } m_Scene = new Scene(this); }
            Scene* GetScene() const { return m_Scene; }

        protected:
            struct RenderPreferences
            {
                i32 MaxLights;
            };
            struct RenderStats
            {
                i64 FrameID;
            };
            RenderPreferences m_Prefs;
            RenderStats m_Stats;
            bool m_DoRecompileAllShaders;
        
            bool m_DefaultTextureNeedsUpdate;
            Texture* m_DefaultTexture;
            Scalar m_DefaultTexturePhase;
            void UpdateDefaultTexture();
        
            Scene* m_Scene;
            
            UniformBuffer* m_EngineUniforms;
            RenderUniformSet* m_RendererUniforms;

            Rasterizer* m_Raster;
    };
};
#endif
