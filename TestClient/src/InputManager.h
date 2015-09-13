#pragma once
#include <Math/Math.h>
#include <vector>
using namespace std;
using namespace Silk;

namespace TestClient
{
    class InputManager
    {
        public:
            InputManager();
            ~InputManager();
            
            void Initialize(i32 ButtonCount);
        
            void Update(Scalar dt);
        
            void OnButtonDown(i32 ButtonID);
            void OnButtonUp  (i32 ButtonID);
            bool IsButtonDown(i32 ButtonID) const { return m_ButtonDurations[ButtonID] != -1.0f; }
            Scalar GetButtonDownDuration(i32 ButtonID) const { return m_ButtonDurations[ButtonID]; }
            void SetMouseButtonIDs(i32 Left,i32 Right,i32 Middle = -1);
        
        protected:
            vector<Scalar> m_ButtonDurations;
            i32 m_MouseLeftID;
            i32 m_MouseMiddleID;
            i32 m_MouseRightID;
    };
};

