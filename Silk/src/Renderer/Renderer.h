#ifndef SILK_RENDERER_H
#define SILK_RENDERER_H
#include <Renderer/RenderObject.h>
#include <Renderer/Camera.h>
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Renderer/UniformBufferTypes.h>

namespace Silk
{
    class UniformBuffer;
    class Renderer
    {
        public:
            Renderer(Rasterizer* Raster);
            virtual ~Renderer();
        
            Rasterizer* GetRasterizer()               { return m_Raster; }
        
            Texture*       GetDefaultTexture       ();
            UniformBuffer* GetEngineUniformBuffer  () { return m_EngineUniforms                 ; }
            UniformBuffer* GetRendererUniformBuffer() { return m_RendererUniforms->GetUniforms(); }
        
            void SetActiveCamera(Camera* c) { m_ActiveCamera = c;    }
            Camera* GetActiveCamera() const { return m_ActiveCamera; }
        
            void UpdateUniforms();
            void Render(i32 PrimType);

            RenderObject* CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene = true);
            Material    * CreateMaterial();
            void Destroy(Material    * Mat);
            void Destroy(RenderObject* Obj);

            void AddRenderObject(RenderObject* Object);
            void RemoveRenderObject(RenderObject* Object) { m_ObjectList->RemoveObject(Object);  }
            void AddToUpdateList(RenderObject* Object)    { m_UpdatedObjects->AddObject(Object); }
        
            void SetMaxLights(i32 MaxLights) { m_Prefs.MaxLights = MaxLights; m_DoRecompileAllShaders = true; }
            i32 GetMaxLights() { return m_Prefs.MaxLights; }

        protected:
            struct RenderPreferences
            {
                i32 MaxLights;
            };
            RenderPreferences m_Prefs;
            bool m_DoRecompileAllShaders;
        
            bool m_DefaultTextureNeedsUpdate;
            Texture* m_DefaultTexture;
            Scalar m_DefaultTexturePhase;
            void UpdateDefaultTexture();
        
            Camera* m_ActiveCamera;
            
            UniformBuffer* m_EngineUniforms;
            RenderUniformSet* m_RendererUniforms;

            ObjectList* m_ObjectList;     //Contains all objects
            ObjectList* m_UpdatedObjects; //clears every frame
            Rasterizer* m_Raster;
    };
};
#endif
