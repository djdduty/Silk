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
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Lighting Test"; }
        
        protected:
            ObjLoader   * m_ObjLoader;
            RenderObject* m_Object   ;
            Mesh        * m_Mesh     ;
            Texture     * m_Diffuse  ;
            Material    * m_Material ;
    };
};

