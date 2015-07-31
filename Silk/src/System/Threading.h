#pragma once
#include <System/SilkTypes.h>

#ifdef __APPLE__

#include <pthread.h>
#include <semaphore.h>
#include <mach/task.h>
#include <mach/semaphore.h>
#include <machine/param.h>

#include <atomic>
using namespace std;

namespace Silk
{
    void SetThreadAffinity(i32 ThreadID);
    class WorkerThread;
    
    class Thread
    {
        public:
            Thread(WorkerThread* Worker);
            ~Thread();
        
            bool Start();
            void Run  ();
            bool Stop ();
        
        protected:
            pthread_t m_Thread;
            WorkerThread* m_Worker;
    };
    
    class Mutex
    {
        public:
            Mutex();
            ~Mutex();
        
            void Lock();
            bool TryLock();
            void Unlock();
        
        protected:
            friend class ThreadCondition;
            pthread_mutex_t m_Mtx;
    };
    
    class Semaphore
    {
        public:
            Semaphore(i32 Value);
            ~Semaphore();
        
            i32 GetValue();
            void Increment();
            void Wait();
        
        protected:
            semaphore_t m_Sem;
            i32 m_Value;
    };
    
    template <typename T>
    class LockingPtr
    {
        public:
            LockingPtr(volatile T& Obj,Mutex& Mtx) : m_Obj(const_cast<T*>(&Obj)), m_Mtx(&Mtx) { Mtx.Lock(); }
            ~LockingPtr() { m_Mtx->Unlock(); }
    
            T& operator* () { return *m_Obj; }
            T* operator->() { return  m_Obj; }
        
        protected:
           T* m_Obj;
           Mutex* m_Mtx;
    };
    
    class ThreadCondition
    {
        public:
            ThreadCondition();
            ~ThreadCondition();
        
            void Signal();
            void WaitSignal(bool CustomPredicateTestValue = false,bool *CustomPredicate = 0);
        
        protected:
            bool m_Predicate;
            pthread_cond_t m_Cond;
            Mutex m_Mutex;
    };
    
};

#elif _WIN32
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

namespace Silk
{
    void SetThreadAffinity(i32 ThreadID);
    class WorkerThread;
    
    class Thread
    {
        public:
            Thread(WorkerThread* Worker);
            ~Thread();
        
            bool Start();
            void Run  ();
            bool Stop ();
        
        protected:
            thread* m_Thread;
            WorkerThread* m_Worker;
    };
    
    class Mutex
    {
        public:
            Mutex();
            ~Mutex();
        
            void Lock();
            bool TryLock();
            void Unlock();
        
        protected:
            friend class ThreadCondition;
			friend class Semaphore;
			 mutex m_Mtx;
    };
    
    class Semaphore
    {
        public:
            Semaphore(i32 Value);
            ~Semaphore();
        
            i32 GetValue();
            void Increment();
            void Wait();
        
        protected:
			Mutex m_Mtx;
			condition_variable m_Condition;
            i32 m_Value;
    };
    
    template <typename T>
    class LockingPtr
    {
        public:
            LockingPtr(volatile T& Obj,Mutex& Mtx) : m_Obj(const_cast<T*>(&Obj)), m_Mtx(&Mtx) { Mtx.Lock(); }
            ~LockingPtr() { m_Mtx->Unlock(); }
    
            T& operator* () { return *m_Obj; }
            T* operator->() { return  m_Obj; }
        
        protected:
           T* m_Obj;
           Mutex* m_Mtx;
    };
    
    class ThreadCondition
    {
        public:
            ThreadCondition();
            ~ThreadCondition();
        
            void Signal();
            void WaitSignal();
        
        protected:
            bool m_Predicate;
			condition_variable m_Cond;
            Mutex m_Mutex;
    };
    
};
#endif