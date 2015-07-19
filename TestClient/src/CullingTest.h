#pragma once
#include <Test.h>

#define GROUND_TEXTURE_SCALE 1.0
#define MIN_PARALLAX_LAYERS 10
#define MAX_PARALLAX_LAYERS 15

namespace TestClient
{
    class ObjLoader;
    class CullingTest : public Test
    {
        public:
            CullingTest();
            ~CullingTest();
        
            virtual void Initialize();
            void LoadMeshes();
            void LoadMaterial();
            void LoadLights();
        
            RenderObject* AddLight(LightType Type,const Vec3& Pos);
            i32 AddMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count = 1);
            Texture * LoadTexture(const char* Path,bool FlipXZ = false);
            Material* AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse,const char* Normal = 0,const char* Parallax = 0);
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Culling Test"; }
        
        protected:
            ObjLoader   * m_ObjLoader;
        
            vector<RenderObject*> m_Meshes       ;
            vector<Material    *> m_Materials    ;
            vector<RenderObject*> m_Lights       ;
            vector<RenderObject*> m_LightMeshes  ;
    };
};

