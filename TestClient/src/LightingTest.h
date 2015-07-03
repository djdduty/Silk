#pragma once
#include <Test.h>

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
        
        protected:
            ObjLoader   * m_ObjLoader;
            RenderObject* m_Object   ;
            RenderObject* m_DisplayL0;
            RenderObject* m_DisplayL1;
            Mesh        * m_Mesh     ;
            Mesh        * m_LDispMesh;
            Texture     * m_Diffuse  ;
            Material    * m_Material ;
            Material    * m_LDispMat ;
            RenderObject* m_Light0   ;
            RenderObject* m_Light1   ;
    };
};

