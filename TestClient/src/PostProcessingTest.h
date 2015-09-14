#pragma once
#include <Test.h>

#define GROUND_TEXTURE_SCALE 25.0f

namespace TestClient
{
    class PostProcessingTest : public Test
    {
        public:
            PostProcessingTest();
            ~PostProcessingTest();
        
            virtual void Initialize();
            void LoadMeshes();
            void LoadMaterial();
            void LoadLights();
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(800,600); }
            virtual const char* GetTestName() const { return "Post Processing Test"; }
        
        protected:
            Texture* m_RTT;
    };
};