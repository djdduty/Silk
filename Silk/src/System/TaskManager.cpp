/*
 *  TaskManager.cpp
 *
 *  Created by Michael DeCicco on 9/15/14.
 *
 */

#include "TaskManager.h"
#include "TaskTypes.h"

#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <mach/mach_init.h>
#endif

#include <thread>
using namespace std;

namespace Silk
{
    Task::Task(Task* Parent) : m_Type(TT_COUNT), m_Parent(Parent)
    {
        m_DependenciesLeft = 0;
        if(Parent) Parent->m_DependenciesLeft++;
        m_Timestamp = 0.0f;
    }
    Task::~Task()
    {
    }
    bool Task::IsReady(Scalar Tm) const
    {
        return m_DependenciesLeft == 0;
    }
    WorkerThread::WorkerThread(TaskManager* Manager) : m_Shutdown(false), m_DidStop(false), m_IsIdling(false), m_ShouldWake(false), m_Manager(Manager), m_Thread(new Thread(this))
    {
    }
    WorkerThread::~WorkerThread()
    {
        delete m_Thread;
    }
    bool WorkerThread::Start()
    {
        return m_Thread->Start();
    }
    void WorkerThread::Stop()
    {
        m_Shutdown = true;
    }
    void WorkerThread::Idle()
    {
        m_IdlingMutex.Lock();
        m_IsIdling = true;
        m_IdlingMutex.Unlock();
        
        m_Sleep.Signal();
        m_Wake .WaitSignal();
        
        m_IdlingMutex.Lock();
        m_IsIdling = false;
        m_IdlingMutex.Unlock();
        
        m_ShouldWake = false;
    }
    bool WorkerThread::IsIdling(bool DoLock)
    {
        if(!DoLock) return m_IsIdling;
        
        m_IdlingMutex.Lock();
        bool r = m_IsIdling;
        m_IdlingMutex.Unlock();
        return r;
    }
    void WorkerThread::Run()
    {
        m_LocalTimer.Start();
        while(!m_Shutdown)
        {
            /* Idle */
            m_LocalTime = m_LocalTimer;
            Idle();
            m_LastFrameIdleDuration = m_LocalTimer - m_LocalTime;
            m_LocalTime = m_LocalTimer;
            
            /* Do work */
            Time FrameBegin = m_LocalTimer;
            Task* T = m_Manager->GetSingleTask(-1,m_ID + 1);
            while(T)
            {
                /* Perform task */
                T->_Run(m_LocalTimer);
                T->m_Duration = m_LocalTimer - T->m_Timestamp;
                
                /* Get task */
                m_LocalTime = m_LocalTimer;
                T = m_Manager->GetSingleTask(-1,m_ID + 1);
            }
            m_LastFrameDuration = m_LocalTimer - FrameBegin;
        }
        m_DidStop = true;
    }
    void WorkerThread::WaitForWake()
    {
        while(m_ShouldWake) { m_Wake.Signal(); }
    }
    
    TaskContainer::TaskContainer() : m_ThreadDifferenceSampleCount(20), m_DurationSampleCount(20), m_ThreadTimeQuota(1.0f / 60.0f),
                                     m_MaxThreadQuotaExcessPercent(0.0f)
    {
        i32 tCount = std::thread::hardware_concurrency();
        for(i32 i = 0;i < tCount;i++)
        {
            for(i32 t = 0;t < TT_COUNT;t++) { m_Tasks[t].push_back(vector<Task*>()); m_TaskMemory[t].push_back(vector<Task*>()); }
            
            m_ThreadDifferenceSamples.push_back(vector<Scalar>());
            m_ProjectedThreadDurations.push_back(0.0f);
            m_ThreadDurations.push_back(0.0f);
            m_AverageThreadDifference.push_back(0.0f);
        }
    }
    TaskContainer::~TaskContainer()
    {
    }
    void TaskContainer::BeginFrame()
    {
        for(i32 i = 0;i < m_ProjectedThreadDurations.size();i++) m_ProjectedThreadDurations[i] = 0.0f;
        m_ProjectedThreadDurations[0] += m_TaskManager->GetLastMainThreadWorkDuration();
    }
    void TaskContainer::EndFrame()
    {
        m_TaskManager->WorkUntilDone();
        m_TaskManager->WaitForThreads();
        
        /*
         * Compute task duration and thread duration averages
         */
        //For each thread
        for(i32 th = 0;th < m_ThreadDurations.size();th++)
        {
            //Post idle duration
            if(th > 0) PostTaskDuration(TT_THREAD_IDLE_TIME,m_TaskManager->GetThread(th - 1)->GetLastFrameIdleDuration());
            
            //For each task type
            for(i32 i = 0;i < TT_COUNT;i++)
            {
                m_Tasks[i][th].clear();
                
                //For each task
                for(i32 t = 0;t < m_TaskMemory[i][th].size();t++)
                {
                    Task* T = m_TaskMemory[i][th][t];
                    PostTaskDuration(T->m_Type,T->m_Duration);
                    delete T;
                }
                m_TaskMemory[i][th].clear();
                
                Scalar TotalDuration = 0.0f;
                if(m_TaskDurationSamples[i].size() > 0)
                {
                    for(i32 t = 0;t < m_TaskDurationSamples[i].size();t++)
                    {
                        TotalDuration += m_TaskDurationSamples[i][t];
                    }
                    m_TaskAvgTimes[i] = TotalDuration / Scalar(m_TaskDurationSamples[i].size());
                }
            }
            
            if(th > 0) PostThreadDuration(th,m_TaskManager->GetThread(th - 1)->GetLastFrameDuration());
            else PostThreadDuration(0,m_TaskManager->GetLastMainThreadWorkDuration());
        }
    }
    
    void TaskContainer::AddTask(Task* T,bool AllowOnMainThread)
    {
        Scalar AvgDuration = GetAverageTaskDuration((TASK_TYPE)T->m_Type);
        
        //Find the thread with the smallest projected execution duration
        i32 ThreadWithMaximumSpace = AllowOnMainThread ? 0 : 1;
        for(i32 i = ThreadWithMaximumSpace;i < m_ProjectedThreadDurations.size();i++)
        {
            if(m_ProjectedThreadDurations[i] < m_ProjectedThreadDurations[ThreadWithMaximumSpace]) ThreadWithMaximumSpace = i;
        }
        
        m_ProjectedThreadDurations[ThreadWithMaximumSpace] += AvgDuration;
        
        m_TaskMemory[T->m_Type][ThreadWithMaximumSpace].push_back(T);
        m_Tasks     [T->m_Type][ThreadWithMaximumSpace].push_back(T);
    }
    bool TaskContainer::HasWork(i32 ThreadID) const
    {
        for(i32 i = 0;i < TT_COUNT;i++)
        {
            for(i32 t = 0;t < m_Tasks[i][ThreadID].size();t++)
            {
                Task* T = m_Tasks[i][ThreadID][t];
                
                Scalar Tm = 0.0f;
                if(ThreadID == 0) Tm = m_TaskManager->GetRunTime();
                else Tm = m_TaskManager->GetThread(ThreadID)->GetLocalTime();
                
                if(T->IsReady(Tm)) return true;
            }
        }
        return false;
    }
    Task* TaskContainer::GetNextTask(i32 DesiredTaskType,i32 ThreadID)
    {
        Scalar LocalTm = ThreadID == 0 ? m_TaskManager->GetRunTime() : m_TaskManager->GetThread(ThreadID - 1)->GetLocalTime();
        
        if(DesiredTaskType == -1)
        {
            //Return the first available task of any type that belongs to the thread
            for(i32 i = 0;i < TT_COUNT;i++)
            {
                for(i32 t = 0;t < m_Tasks[i][ThreadID].size();t++)
                {
                    Task* T = m_Tasks[i][ThreadID][t];
                    if(!T->IsReady(LocalTm)) continue;
                    m_Tasks[i][ThreadID].erase(m_Tasks[i][ThreadID].begin() + t);
                    return T;
                }
            }
            return 0;
        }
        
        //Return the first available task of the desired type that belongs to the thread
        for(i32 t = 0;t < m_Tasks[DesiredTaskType][ThreadID].size();t++)
        {
            Task* T = m_Tasks[DesiredTaskType][ThreadID][t];
            if(!T->IsReady(LocalTm)) continue;
            m_Tasks[DesiredTaskType][ThreadID].erase(m_Tasks[DesiredTaskType][ThreadID].begin() + t);
            return T;
        }
        
        return 0;
    }

    void TaskContainer::PostTaskDuration(TASK_TYPE Type,Scalar Duration)
    {
        if(m_TaskDurationSamples[Type].size() - 1 == m_DurationSampleCount)
        {
            for(i32 i = 0;i < m_TaskDurationSamples[Type].size() - 1;i++)
            {
                m_TaskDurationSamples[Type][i] = m_TaskDurationSamples[Type][i + 1];
            }
            m_TaskDurationSamples[Type][m_TaskDurationSamples[Type].size() - 1] = Duration;
        }
        else m_TaskDurationSamples[Type].push_back(Duration);
    }
    void TaskContainer::PostThreadDuration(i32 ThreadID,Scalar Duration)
    {
        if(Duration > m_ThreadTimeQuota)
        {
            Scalar PercentExcess = (Duration / m_ThreadTimeQuota);
            if(PercentExcess > m_MaxThreadQuotaExcessPercent)
            {
                m_MaxThreadQuotaExcessPercent = PercentExcess;
                Scalar ThreadFreq = 1.0f / Duration;
                Scalar TargetFreq = 1.0f / m_ThreadTimeQuota;
                ERROR("Warning: Heavy load. Thread[%d] frequency (%0.2f Hz) below target frequency (%0.2f Hz) by %0.2f Hz.\n",ThreadID,ThreadFreq,TargetFreq,TargetFreq - ThreadFreq);
            }
        }
        
        Scalar Difference = GetThreadTimeDifference(ThreadID);
        if(m_ThreadDifferenceSamples[ThreadID].size() - 1 == m_ThreadDifferenceSampleCount)
        {
            //Update sample array and accumulate durations
            Scalar Total = Difference;
            for(i32 i = 0;i < m_ThreadDifferenceSamples[ThreadID].size() - 1;i++)
            {
                m_ThreadDifferenceSamples[ThreadID][i] = m_ThreadDifferenceSamples[ThreadID][i + 1];
                Total += m_ThreadDifferenceSamples[ThreadID][i];
            }
            m_ThreadDifferenceSamples[ThreadID][m_ThreadDifferenceSamples[ThreadID].size() - 1] = Difference;
            
            //Calculate average
            m_AverageThreadDifference[ThreadID] = Total / Scalar(m_ThreadDifferenceSampleCount);
        }
        else
        {
            m_ThreadDifferenceSamples[ThreadID].push_back(Difference);
            
            //Calculate average
            Scalar Total = 0.0f;
            for(i32 i = 0;i < m_ThreadDifferenceSamples[ThreadID].size();i++) Total += m_ThreadDifferenceSamples[ThreadID][i];
            m_AverageThreadDifference[ThreadID] = Total / Scalar(m_ThreadDifferenceSamples[ThreadID].size() + 1);
        }
        
        m_ThreadDurations[ThreadID] = Duration;
    }
    
    TaskManager::TaskManager()
    {
        m_TaskContainer.m_TaskManager = this;
        m_IsShuttingDown = false;
        i32 pCount = GetCoreCount() - 1;
        
        for(i32 i = 0;i < pCount;i++)
        {
            WorkerThread* w = new WorkerThread(this);
            w->m_ID = (i32)m_Threads.size();
            m_Threads.push_back(w);
            m_ThreadFrameDurations.push_back(0.0f);
        }
        for(i32 i = 0;i < pCount;i++) m_Threads[i]->Start();
        
        //And one for the main thread
        m_ThreadFrameDurations.push_back(0.0f);
        
        SetThreadAffinity(pCount);
    }
    TaskManager::~TaskManager()
    {
        for(i32 i = 0;i < m_Threads.size();i++) m_Threads[i]->Stop();
        WakeThreads();
        for(i32 i = 0;i < m_Threads.size();i++) delete m_Threads[i];
    }
    void TaskManager::Shutdown()
    {
        for(i32 i = 0;i < m_Threads.size();i++) m_Threads[i]->Stop();
        m_IsShuttingDown = true;
    }
    void TaskManager::WaitForShutdown()
    {
        for(i32 i = 0;i < m_Threads.size();i++)
        {
            ERROR("Waiting for engine thread %d to stop...\n",i);
            while(!m_Threads[i]->IsStopped()) { }
        }
    }

    i32 TaskManager::GetCoreCount() const
    {
        return std::thread::hardware_concurrency();
    }
    void TaskManager::AddTask(Task* T,bool AllowOnMainThread)
    {
        m_TaskContainer.AddTask(T,AllowOnMainThread);
    }
    Task* TaskManager::GetSingleTask(i32 DesiredTaskType,i32 ThreadID)
    {
        if(m_IsShuttingDown) return 0;
        return m_TaskContainer.GetNextTask(DesiredTaskType,ThreadID);
    }
    void TaskManager::WorkUntilDone(i32 DesiredTaskType)
    {
        static Timer ftmr;
        m_LastMainThreadWorkDuration = ftmr;
        ftmr.Stop();
        ftmr.Start();
        
        Task* T = GetSingleTask(DesiredTaskType);
        while(T)
        {
            /* Perform task */
            Time Begin = m_Timer;
            T->_Run(Begin);
            T->m_Duration  = Begin - T->m_Timestamp;
            
            /* Get task */
            T = GetSingleTask(DesiredTaskType);
        }
    }

    void TaskManager::WakeThreads()
    {
        for(i32 i = 0;i < m_Threads.size();i++)
        {
            if(m_Threads[i]->IsIdling())
            {
                m_Threads[i]->m_ShouldWake = true;
                m_Threads[i]->m_Wake.Signal();
                m_Threads[i]->WaitForWake();
            }
        }
    }
    void TaskManager::WaitForThreads() const
    {
        for(i32 i = 0;i < m_Threads.size();i++)
        {
            if(!m_Threads[i]->IsIdling()) m_Threads[i]->m_Sleep.WaitSignal(false,&m_Threads[i]->m_IsIdling);
        }
    }
    bool TaskManager::HasWork(i32 ThreadID) const
    {
        if(m_IsShuttingDown) return false;
        return m_TaskContainer.HasWork(ThreadID);
    }
    void TaskManager::EndFrame()
    {
        m_TaskContainer.EndFrame();
        m_ThreadFrameDurations[0] = m_LastMainThreadWorkDuration;
        
        for(i32 i = 0;i < TT_COUNT;i++)
        {
            m_AverageTaskDurations[i] = m_TaskContainer.GetAverageTaskDuration((TASK_TYPE)i);
        }
        
        for(i32 i = 0;i < m_Threads.size();i++)
        {
            m_ThreadFrameDurations[i + 1] = m_Threads[i]->GetLastFrameDuration();
        }
    }
};