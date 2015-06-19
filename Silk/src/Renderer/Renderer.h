<<<<<<< HEAD
#pragma once
#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Raster/Raster.h>
=======
#ifndef SILK_RENDERER_H
#define SILK_RENDERER_H
#include <Renderer/RenderObject.h>
>>>>>>> 70aa414345c205ff8357e85426718a917763f0e5

namespace Silk
{
    class Renderer
    {
        public:
            Renderer(Rasterizer* Raster);
            virtual ~Renderer();
<<<<<<< HEAD
        
            UniformBuffer* GetEngineUniformBuffer()   { return m_EngineUniforms; }
            UniformBuffer* GetRendererUniformBuffer() { return m_RendererUniforms; }
        
        protected:
            UniformBuffer* m_EngineUniforms;
            UniformBuffer* m_RendererUniforms;
=======
            void Render(i32 PrimType);

            RenderObject* CreateRenderObject(RENDER_OBJECT_TYPE Rot, bool AddToScene = true);

            void AddRenderObject(RenderObject* Object)
            {
                if(!Object)
                    return;

                Object->m_ListIndex = m_ObjectList->AddObject(Object);
                Object->m_List = m_ObjectList;
            }
            void RemoveRenderObject(RenderObject* Object) { m_ObjectList->RemoveObject(Object);  }
            void AddToUpdateList(RenderObject* Object)    { m_UpdatedObjects->AddObject(Object); }

        protected:
            ObjectList* m_ObjectList;     //Contains all objects
            ObjectList* m_UpdatedObjects; //clears every frame
            Rasterizer* m_Raster;
>>>>>>> 70aa414345c205ff8357e85426718a917763f0e5
    };
};
#endif
