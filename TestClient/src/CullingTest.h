#pragma once
#include <Test.h>

#define GROUND_TEXTURE_SCALE 1.0
#define MIN_PARALLAX_LAYERS 10
#define MAX_PARALLAX_LAYERS 20
#define NUM_OF_CULL_OBJECTS 10000
#define CULL_OBJECT_SCALE 0.75
#define SPAWN_CUBE_SIZE 200

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
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Culling Test"; }
        
        protected:
    };
};

