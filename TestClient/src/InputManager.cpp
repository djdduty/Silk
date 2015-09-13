#include "InputManager.h"

namespace TestClient
{
    InputManager::InputManager() : m_MouseLeftID(-1), m_MouseMiddleID(-1), m_MouseRightID(-1)
    {
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
        //Update button times
        for(i32 i = 0;i < m_ButtonDurations.size();i++)
        {
            if(m_ButtonDurations[i] != -1.0f) m_ButtonDurations[i] += dt;
        }
    }
    void InputManager::OnButtonDown(i32 ButtonID)
    {
        m_ButtonDurations[ButtonID] = 0.0f;
        
        //Should something go here?
        if(ButtonID == m_MouseLeftID)
        {
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
    }
};
