#pragma once
#include <Test.h>

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
        
        protected:
    };
};