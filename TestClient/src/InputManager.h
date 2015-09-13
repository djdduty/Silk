#pragma once
#include <Math/Math.h>
#include <vector>
using namespace std;
namespace Silk
{
    class Renderer;
};
using namespace Silk;

namespace TestClient
{
    class InputManager
    {
        public:
            InputManager(Renderer* r);
            ~InputManager();
            
            void Initialize(i32 ButtonCount);
        
            void Update(Scalar dt);
    
            void SetCursorPosition(const Vec2& p);
            Vec2 GetCursorPosition() const { return m_CursorPosition; }
            Vec2 GetCursorDelta   () const { return m_CursorPosition - m_LastCursorPosition; }
            Vec2 GetUnBoundedCursorPosition() const { return m_UnBoundedCursorPosition; }
            Vec2 GetUnBoundedCursorDelta   () const { return m_UnBoundedCursorPosition - m_LastUnBoundedCursorPosition; }
            void ResetCursorDelta ()       { m_LastCursorPosition = m_CursorPosition; m_LastUnBoundedCursorPosition = m_UnBoundedCursorPosition; }
        
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
            
            Vec2 m_Resolution;

            Vec2 m_LastUnBoundedCursorPosition;
            Vec2 m_UnBoundedCursorPosition;
            Vec2 m_LastCursorPosition;
            Vec2 m_CursorPosition;
            Vec2 m_RealCursorPosition;
            Renderer* m_Renderer;
    };
};

