#pragma once
#include <Test.h>

#define GROUND_TEXTURE_SCALE 25.0f

#define PARTICLE_COUNT              1000000
#define PARTICLE_SPAWN_POS          Vec3(0,15,0)
#define PARTICLE_SPAWN_RADIUS       3.0
#define PARTICLE_VELOCITY_MIN       Vec3(9.0,15.0, 2.0)
#define PARTICLE_VELOCITY_MAX       Vec3(12.0,20.0, 4.0)
#define PARTICLE_VELOCITY_ROT_AXIS  Vec3(0,1,0)
#define PARTICLE_VELOCITY_ROTATIONS 100.0
#define PARTICLE_MIN_EMIT_TIME      0.0
#define PARTICLE_MAX_EMIT_TIME      100.0
#define PARTICLE_MIN_DURATION       5.0
#define PARTICLE_MAX_DURATION       20.0

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
