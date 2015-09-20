#pragma once
#include <Renderer/Renderer.h>

namespace Silk
{
    class DeferredRenderer : public Renderer
    {
        public:
            DeferredRenderer(Rasterizer* Raster,TaskManager* TaskMgr) : Renderer(Raster, TaskMgr) { }
            virtual void RenderObjects(ObjectList* List,PRIMITIVE_TYPE PrimType);

        private:
    };
};
