#pragma once
#include <Test.h>

namespace TestClient
{
    class ObjLoader;
    class NormalMappingTest : public Test
    {
        public:
            NormalMappingTest();
            ~NormalMappingTest();
        
            virtual void Initialize();
            void LoadMesh();
            void LoadMaterial();
            void LoadLight();
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Normal Mapping Test"; }
        
        protected:
            ObjLoader   * m_ObjLoader;
            RenderObject* m_Object   ;
            Mesh        * m_Mesh     ;
            Mesh        * m_LDispMesh;
            Texture     * m_Diffuse  ;
            Texture     * m_Normal   ;
            Material    * m_Material ;
            Material    * m_LDispMat ;
            vector<RenderObject*> m_LightDisplays;
            vector<RenderObject*> m_Lights       ;
    };
};

