#include <System/Threading.h>
#include <System/TaskManager.h>

#ifdef __APPLE__

#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach_init.h>

namespace Silk
{
    void SetThreadAffinity(i32 ThreadID)
    {
        thread_extended_policy_data_t epolicy;
        epolicy.timeshare = FALSE;
        kern_return_t ret;
        ret = thread_policy_set(mach_thread_self(),THREAD_EXTENDED_POLICY,(thread_policy_t)&epolicy,THREAD_EXTENDED_POLICY_COUNT);
        if(ret != KERN_SUCCESS) ERROR("thread_policy_set returned %d", ret);

        thread_affinity_policy_data_t apolicy;
        apolicy.affinity_tag = ThreadID;
        ret = thread_policy_set(mach_thread_self(),THREAD_AFFINITY_POLICY,(thread_policy_t)&apolicy,THREAD_AFFINITY_POLICY_COUNT);
        if(ret != KERN_SUCCESS) ERROR("thread_policy_set returned %d", ret);
    }
    
    void* ThreadFunc(void* Ptr)
    {
        ((Thread*)Ptr)->Run();
        return 0;
    }
    
    Thread::Thread(WorkerThread* Worker) : m_Worker(Worker)
    {
    }
    Thread::~Thread()
    {
        Stop();
    }
    
    bool Thread::Start()
    {
        return pthread_create(&m_Thread,0,ThreadFunc,this);
    }
    void Thread::Run()
    {
        SetThreadAffinity(m_Worker->GetID());
        m_Worker->Run();
        pthread_exit(0);
    }
    bool Thread::Stop()
    {
        return pthread_join(m_Thread,0);
    }
    
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

#else

#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach_init.h>

namespace Silk
{
    void SetThreadAffinity(i32 ThreadID)
    {
        thread_extended_policy_data_t epolicy;
        epolicy.timeshare = FALSE;
        kern_return_t ret;
        ret = thread_policy_set(mach_thread_self(),THREAD_EXTENDED_POLICY,(thread_policy_t)&epolicy,THREAD_EXTENDED_POLICY_COUNT);
        if(ret != KERN_SUCCESS) ERROR("thread_policy_set returned %d", ret);

        thread_affinity_policy_data_t apolicy;
        apolicy.affinity_tag = ThreadID;
        ret = thread_policy_set(mach_thread_self(),THREAD_AFFINITY_POLICY,(thread_policy_t)&apolicy,THREAD_AFFINITY_POLICY_COUNT);
        if(ret != KERN_SUCCESS) ERROR("thread_policy_set returned %d", ret);
    }
    
    void* ThreadFunc(void* Ptr)
    {
        ((Thread*)Ptr)->Run();
        return 0;
    }
    
    Thread::Thread(WorkerThread* Worker) : m_Worker(Worker)
    {
    }
    Thread::~Thread()
    {
        Stop();
    }
    
    bool Thread::Start()
    {
        return pthread_create(&m_Thread,0,ThreadFunc,this);
    }
    void Thread::Run()
    {
        SetThreadAffinity(m_Worker->GetID());
        m_Worker->Run();
        pthread_exit(0);
    }
    bool Thread::Stop()
    {
        return pthread_join(m_Thread,0);
    }
    
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

#endif
