#pragma once
#include <Test.h>

#define BUTTON_COLOR 0.5,0.5,0.5,1.0

namespace TestClient
{
    class UITest : public Test
    {
        public:
            UITest();
            ~UITest();
        
            virtual void Initialize();
            virtual void Run();
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "User Interface Test"; }
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(1100,120); }
        
        protected:
    };
};