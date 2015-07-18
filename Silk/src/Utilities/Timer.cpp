#include <Utilities/Timer.h>

namespace Silk
{
    Timer::Timer() : m_Started(false)
    {
    }
    Timer::~Timer()
    {
    }

    void Timer::Start()
    {
        if(m_Started) return;
        m_Started = true;
        m_StartTime = std::chrono::high_resolution_clock::now();
    }
    void Timer::Stop()
    {
        m_Started = false;
    }

    Time Timer::ElapsedTime() const
    {
        if(m_Started) return std::chrono::duration<Time>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
        return Scalar(0);
    }
    Timer::operator Time() const
    {
        if(m_Started) return std::chrono::duration<Time>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
        return Scalar(0);
    }
};