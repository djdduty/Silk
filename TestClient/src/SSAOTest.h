#pragma once
#include <Test.h>
#include <Renderer/DeferredRenderer.h>

#define SSAO_KERNEL_SIZE 128

namespace TestClient
{
    class ObjLoader;
    class SSAOTest : public Test
    {
        public:
            SSAOTest();
            ~SSAOTest();
        
            virtual void Initialize();
            void LoadMesh();
            void LoadMaterial();
            void LoadLight();
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "SSAO Test"; }
        
#ifdef __APPLE__
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(800,600); }
#else
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(1280,900); }
#endif

            virtual Renderer* GetPreferredRenderer(Rasterizer* Raster,TaskManager* TaskMng) const { return new DeferredRenderer(Raster,TaskMng); }
        
        protected:
            ObjLoader   * m_ObjLoader;
            RenderObject* m_Object   ;
            Mesh        * m_Mesh     ;
            Mesh        * m_LDispMesh;
            Texture     * m_Diffuse  ;
            Material    * m_Material ;
            Material    * m_LDispMat ;
			Material    * m_NoFxaa   ;
			Material    * m_Final    ;
    };
};

