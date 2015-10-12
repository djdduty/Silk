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
            Vec2 GetCursorPosition    () const { return m_CursorPosition; }
            Vec2 GetCursorDelta       () const { return m_CursorPosition - m_LastCursorPosition; }
            Vec2 GetRealCursorPosition() const { return m_RealCursorPosition; }
            Vec2 GetRealCursorDelta   () const { return m_RealCursorDelta; }
            void ResetCursorDelta     ()       { m_RealCursorDelta = Vec2(0,0); }
        
            void OnButtonDown(i32 ButtonID);
            void OnButtonUp  (i32 ButtonID);
            bool IsButtonDown(i32 ButtonID) const { return m_ButtonDurations[ButtonID] != -1.0f; }
            Scalar GetButtonDownDuration(i32 ButtonID) const { return m_ButtonDurations[ButtonID]; }
            void SetMouseButtonIDs(i32 Left,i32 Right,i32 Middle = -1) { m_MouseLeftID = Left; m_MouseRightID = Right; m_MouseMiddleID = Middle; }
        
        protected:
            vector<Scalar> m_ButtonDurations;
            i32 m_MouseLeftID;
            i32 m_MouseMiddleID;
            i32 m_MouseRightID;
            
            Vec2 m_Resolution;

            Vec2 m_LastCursorPosition;
            Vec2 m_CursorPosition;
            Vec2 m_RealCursorPosition;
            Vec2 m_VirtualCursorPosition;
            Vec2 m_RealCursorDelta;
            Renderer* m_Renderer;
    };
};

