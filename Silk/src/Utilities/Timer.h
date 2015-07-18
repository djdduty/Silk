#pragma once
#include <System/SilkTypes.h>
#include <chrono>

namespace Silk
{
    class Timer
    {
        public:
            Timer();
            ~Timer();
        
            void Start();
            void Stop();
        
            Time ElapsedTime() const;
            operator Time() const;
        
        protected:
            std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
            bool m_Started;
    };
};