#pragma once
#include <Renderer/Renderer.h>

namespace Silk
{
    class DeferredRenderer : public Renderer
    {
        public:
            DeferredRenderer(Rasterizer* Raster,TaskManager* TaskMgr);
            ~DeferredRenderer();
            virtual void RenderObjects(ObjectList* List,PRIMITIVE_TYPE PrimType,bool SendLighting = true);
            void LightPass(RenderObject* l);
        
            void SetPointLightObject        (RenderObject* Obj);
            void SetSpotLightObject         (RenderObject* Obj);
            void SetDirectionalLightObject  (RenderObject* Obj);
            void SetPointLightMaterial      (Material*     Mat);
            void SetSpotLightMaterial       (Material*     Mat);
            void SetDirectionalLightMaterial(Material*     Mat);
            void SetFinalPassMaterial       (Material*     Mat);
            Material* GetPointLightMaterial      () const { return m_PointLightMat      ; }
            Material* GetSpotLightMaterial       () const { return m_SpotLightMat       ; }
            Material* GetDirectionalLightMaterial() const { return m_DirectionalLightMat; }
            Material* GetFinalPassMaterial       () const { return m_FinalPassMat       ; }
        
            virtual Texture* GetLightAccumulationTexture() { return m_LightAccumulationBuffer; }
        
            virtual void OnResolutionChanged();
        
        private:
            RenderObject* m_PointLightObj;
            RenderObject* m_SpotLightObj;
            RenderObject* m_DirectionalLightObj;
            Material*     m_PointLightMat;
            Material*     m_SpotLightMat;
            Material*     m_DirectionalLightMat;
            Material*     m_FinalPassMat;
            Texture*      m_LightAccumulationBuffer;
    };
};
