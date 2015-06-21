#ifndef SILK_RENDERER_H
#define SILK_RENDERER_H
#include <Renderer/RenderObject.h>
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Utilities/PerlinNoise.h>

namespace Silk
{
    class Camera
    {
        public:
            Camera(Vec2 FoV, Scalar NearPlane, Scalar FarPlane) : //Perspective
                m_Transform(Mat4::Identity)
            {
                m_Projection = PerspectiveMultiFov(FoV.x, FoV.y, NearPlane, FarPlane);
            }

            Camera(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom, Scalar Near, Scalar Far) : //Ortho
                m_Transform(Mat4::Identity)
            {
                m_Projection = Orthogonal(Left, Right, Top, Bottom, Near, Far);
            }

            Camera(Mat4 Projection) : m_Projection(Projection), m_Transform(Mat4::Identity) {}
            ~Camera() {}

            Mat4 GetTransform() { return m_Transform; }
            Mat4 GetProjection() { return m_Projection; }

            void SetTransform(Mat4 Transform) { m_Transform = Transform; }
            void SetProjection(Mat4 Projection) { m_Projection = Projection; }
        protected:
            Mat4 m_Transform;
            Mat4 m_Projection;
    };

    class UniformBuffer;
    class Renderer
    {
        public:
            Renderer(Rasterizer* Raster);
            virtual ~Renderer();
        
            Rasterizer* GetRasterizer()               { return m_Raster; }
        
            Texture*       GetDefaultTexture       ();
            PerlinNoise*   GetNoiseGenerator       () { return &m_NoiseGenerator ; }
            UniformBuffer* GetEngineUniformBuffer  () { return m_EngineUniforms  ; }
            UniformBuffer* GetRendererUniformBuffer() { return m_RendererUniforms; }
        
            void Render(i32 PrimType);

            RenderObject* CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene = true);

            void AddRenderObject(RenderObject* Object)
            {
                if(!Object) return;
                    
                Object->m_ListIndex = m_ObjectList->AddObject(Object);
                Object->m_List = m_ObjectList;
            }
            void RemoveRenderObject(RenderObject* Object) { m_ObjectList->RemoveObject(Object);  }
            void AddToUpdateList(RenderObject* Object)    { m_UpdatedObjects->AddObject(Object); }

        protected:
            bool m_DefaultTextureNeedsUpdate;
            Texture* m_DefaultTexture;
            f32 m_DefaultTexturePhase;
            void UpdateDefaultTexture();
            PerlinNoise m_NoiseGenerator;
            
            UniformBuffer* m_EngineUniforms;
            UniformBuffer* m_RendererUniforms;

            ObjectList* m_ObjectList;     //Contains all objects
            ObjectList* m_UpdatedObjects; //clears every frame
            Rasterizer* m_Raster;
    };
};
#endif
