#pragma once
#include <Test.h>
#include <Renderer/DeferredRenderer.h>

namespace TestClient
{
    class ObjLoader;
    class LightingTest : public Test
    {
        public:
            LightingTest();
            ~LightingTest();
        
            virtual void Initialize();
            void LoadMesh();
            void LoadMaterial();
            void LoadLight();
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Lighting Test"; }
            virtual Renderer* GetPreferredRenderer(Rasterizer* Raster, TaskManager* TaskMng) const { return new DeferredRenderer(Raster,TaskMng); }
        
        protected:
            ObjLoader   * m_ObjLoader;
            RenderObject* m_Object   ;
            Mesh        * m_Mesh     ;
            Mesh        * m_LDispMesh;
            Texture     * m_Diffuse  ;
            Material    * m_Material ;
            Material    * m_LDispMat ;
            vector<RenderObject*> m_LightDisplays;
            vector<RenderObject*> m_Lights       ;
    };
};

