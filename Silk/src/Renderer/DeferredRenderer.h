#pragma once
#include <Renderer/Renderer.h>

namespace Silk
{
    class DeferredRenderer : public Renderer
    {
        public:
            DeferredRenderer(Rasterizer* Raster,TaskManager* TaskMgr);
            virtual void RenderObjects(ObjectList* List,PRIMITIVE_TYPE PrimType,bool SendLighting = true);
            void LightPass(Light* l);
        
            void SetPointLightObject        (RenderObject* Obj);
            void SetSpotLightObject         (RenderObject* Obj);
            void SetDirectionalLightObject  (RenderObject* Obj);
            void SetPointLightMaterial      (Material*     Mat);
            void SetSpotLightMaterial       (Material*     Obj);
            void SetDirectionalLightMaterial(Material*     Obj);
        
        private:
            RenderObject* m_PointLightObj;
            RenderObject* m_SpotLightObj;
            RenderObject* m_DirectionalLightObj;
            Material*     m_PointLightMat;
            Material*     m_SpotLightMat;
            Material*     m_DirectionalLightMat;
    };
};
