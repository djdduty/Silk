#pragma once
#include <Test.h>

#define GROUND_TEXTURE_SCALE 25.0f

namespace TestClient
{
    class ObjLoader;
    class RTTTest : public Test
    {
        public:
            RTTTest();
            ~RTTTest();
        
            virtual void Initialize();
            void LoadMeshes();
            void LoadMaterial();
            void LoadLights();
        
            virtual void Run();
        
            virtual void Shutdown();
        
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(800,600); }
            virtual const char* GetTestName() const { return "RTT Test"; }
        
        protected:
            Texture* m_RTT;
    };
};

