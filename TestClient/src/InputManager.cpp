#include "InputManager.h"
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>
#include <UI/UI.h>

namespace TestClient
{
    InputManager::InputManager(Renderer* r) : m_MouseLeftID(-1), m_MouseMiddleID(-1), m_MouseRightID(-1), m_Renderer(r)
    {
        m_Resolution = r->GetRasterizer()->GetContext()->GetResolution();
    }
    InputManager::~InputManager()
    {
    }
    void InputManager::Initialize(i32 ButtonCount)
    {
        for(i32 i = 0;i < ButtonCount;i++) m_ButtonDurations.push_back(-1.0f);
    }
    void InputManager::Update(Scalar dt)
    {
        m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        
        //Update button times
        for(i32 i = 0;i < m_ButtonDurations.size();i++)
        {
            if(m_ButtonDurations[i] != -1.0f) m_ButtonDurations[i] += dt;
        }
    }
    void InputManager::SetCursorPosition(const Vec2& p)
    {
        UIManager* UI = m_Renderer->GetUIManager();
        if(!UI) return;
        
        m_RealCursorDelta = p - m_RealCursorPosition;
        m_RealCursorPosition = p;
        
        m_VirtualCursorPosition += m_RealCursorDelta;
        
        //Lock cursor to window
        Vec2 Temp0 = m_VirtualCursorPosition;
        Vec2 Temp1 = m_VirtualCursorPosition;
        
        if(Temp0.x < 0                  ) Temp0.x = 0             ;
        else if(Temp0.x > m_Resolution.x) Temp0.x = m_Resolution.x;
        if(Temp0.y < 0                  ) Temp0.y = 0             ;
        else if(Temp0.y > m_Resolution.y) Temp0.y = m_Resolution.y;
        
        m_VirtualCursorPosition = Temp0;
        
        //Transform to normalized window coordinates
        if(Temp0.x != 0) Temp0.x /= m_Resolution.x;
        if(Temp0.y != 0) Temp0.y /= m_Resolution.y;
        if(Temp1.x != 0) Temp1.x /= m_Resolution.x;
        if(Temp1.y != 0) Temp1.y /= m_Resolution.y;
        
        //Transform to "virtual" window coordinates
        Camera* Cam = UI->GetCamera();
        Vec4 Ortho  = Cam->GetOrthoRect();
        Vec3 CamPos = Cam->GetTransform().GetTranslation();
        
        m_LastCursorPosition = m_CursorPosition;
        m_CursorPosition = CamPos.xy() + Ortho.xy() + (Ortho.zw() * Temp0);
        
        UI->OnMouseMove();
    }
    void InputManager::OnButtonDown(i32 ButtonID)
    {
        m_ButtonDurations[ButtonID] = 0.0f;
        
        //Should something go here?
        if(ButtonID == m_MouseLeftID)
        {
            m_Renderer->GetUIManager()->OnMouseDown();
        }
        else if(ButtonID == m_MouseRightID)
        {
        }
        else if(ButtonID == m_MouseMiddleID)
        {
        }
    }
    void InputManager::OnButtonUp(i32 ButtonID)
    {
        m_ButtonDurations[ButtonID] = -1.0f;
        if(ButtonID == m_MouseLeftID)
        {
            m_Renderer->GetUIManager()->OnMouseUp();
        }
        else if(ButtonID == m_MouseRightID)
        {
        }
        else if(ButtonID == m_MouseMiddleID)
        {
        }
    }
};
