#include <System/Threading.h>


#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach_init.h>
#endif

namespace Silk
{
    Mutex::Mutex() : m_Mtx(PTHREAD_MUTEX_INITIALIZER)
    {
    }
    Mutex::~Mutex()
    {
        pthread_mutex_destroy(&m_Mtx);
    }

    void Mutex::Lock()
    {
        pthread_mutex_lock(&m_Mtx);
    }
    bool Mutex::TryLock()
    {
        return pthread_mutex_trylock(&m_Mtx) == 0;
    }
    void Mutex::Unlock()
    {
        pthread_mutex_unlock(&m_Mtx);
    }
    #ifdef __APPLE__
    Semaphore::Semaphore(i32 Value)
    {
        semaphore_create(mach_task_self(),&m_Sem,SYNC_POLICY_FIFO,Value);
        m_Value = Value;
    }
    Semaphore::~Semaphore()
    {
        semaphore_destroy(mach_task_self(),m_Sem);
    }

    i32 Semaphore::GetValue()
    {
        return m_Value;
    }
    void Semaphore::Increment()
    {
        semaphore_signal(m_Sem);
        m_Value++;
    }
    void Semaphore::Wait()
    {
        semaphore_wait(m_Sem);
        m_Value--;
    }
    #endif
    
    ThreadCondition::ThreadCondition()
    {
        pthread_cond_init(&m_Cond,0);
        m_Predicate = true;
    }
    ThreadCondition::~ThreadCondition()
    {
        pthread_cond_destroy(&m_Cond);
    }
    void ThreadCondition::Signal()
    {
        m_Mutex.Lock();
        m_Predicate = false;
        
        pthread_cond_signal(&m_Cond);
        m_Mutex.Unlock();
    }
    void ThreadCondition::WaitSignal()
    {
        m_Mutex.Lock();
        m_Predicate = true;
        while(m_Predicate)
        {
            pthread_cond_wait(&m_Cond,&m_Mutex.m_Mtx);
        }
        m_Predicate = true;
        m_Mutex.Unlock();
    }
};