#pragma once

#include <System/SilkTypes.h>
#include <System/Threading.h>
#include <System/TaskTypes.h>
#include <Utilities/Timer.h>
#include <vector>
#include <queue>
using namespace std;


namespace Silk
{
    
    class TaskInfo
    {
        public:
            TaskInfo(i32 Type,Time Begin,Time End) : TaskType(Type), BeginTime(Begin), EndTime(End) { }
            ~TaskInfo() { }
            i32 TaskType;
            Time BeginTime;
            Time EndTime;
    };
    
    class Task
    {
        public:
            Task(Task* Parent = 0);
            virtual ~Task();
        
            bool IsReady(Scalar Tm) const;
            void _Run(Time BeginTime) { m_Timestamp = BeginTime; Run(); if(m_Parent) m_Parent->OnDependantCompleted(); }
        
            virtual void Run() = 0;
            void SetParent(Task* Parent) { m_Parent = Parent; }
        
            TASK_TYPE m_Type;
            Time m_Timestamp;
            Time m_Duration;
        
        protected:
            friend class TaskManager;
            void OnDependantCompleted() { m_DependenciesLeft--; }
            i32 m_ID;
            Task* m_Parent;
            i32 m_DependenciesLeft;
    };
    
    class TaskManager;
    class WorkerThread
    {
        public:
            WorkerThread(TaskManager* Manager);
            ~WorkerThread();
        
            bool Start();
            void Stop();
        
            void Idle();
            bool IsIdling ();
            bool IsStopped() const { return m_DidStop ; }
            void Run();
        
            Time GetLocalTime() const { return m_LocalTime; }
            Scalar GetLastFrameDuration() const { return m_LastFrameDuration; }
            Scalar GetLastFrameIdleDuration() const { return m_LastFrameIdleDuration; }
        
        protected:
            friend class TaskManager;
            bool m_Shutdown;
            bool m_DidStop;
            bool m_IsIdling;

            i32 m_ID;
            Timer m_LocalTimer;
            Time m_LocalTime;
            Scalar m_LastFrameDuration;
            Scalar m_LastFrameIdleDuration;
            TaskManager* m_Manager;
            pthread_t m_Thread;
            ThreadCondition m_Wake;
            ThreadCondition m_Sleep;
            Mutex m_IdlingMutex;
    };
    
    class TaskContainer
    {
        public:
            TaskContainer();
            ~TaskContainer();
        
            void BeginFrame();
            void EndFrame();
        
            void AddTask(Task* T,bool AllowOnMainThread);
            bool HasWork(i32 ThreadID) const;
            Task* GetNextTask(i32 ThreadID,i32 DesiredTaskType = -1);
        
            void SetTimestep(Scalar Timestep) { m_ThreadTimeQuota = Timestep; }
        
            void SetAverageTaskDurationSampleCount(i32 Count) { m_DurationSampleCount = Count; }
            void PostTaskDuration(TASK_TYPE Type,Scalar Duration);
        
            Scalar GetAverageTaskDuration(TASK_TYPE Type) const { return m_TaskAvgTimes[Type]; }
            Scalar GetThreadTimeUntilQuota(i32 ThreadID) const { return m_ThreadTimeQuota - m_ProjectedThreadDurations[ThreadID]; }
        
            /* Negative values mean the thread consumed too much time to maintain the target framerate */
            Scalar GetThreadTimeDifference(i32 ThreadID) const { return m_ThreadTimeQuota - m_ThreadDurations[ThreadID]; }
            Scalar GetAverageThreadTimeDifference(i32 ThreadID) const { return m_AverageThreadDifference[ThreadID]; }
            void SetAverageThreadTimeDifferenceSampleCount(i32 Count) { m_ThreadDifferenceSampleCount = Count; }
            void PostThreadDuration(i32 ThreadID,Scalar Duration);
        
        protected:
            friend class TaskManager;
            
            /* Maximum recorded excess in thread duration as percentage of quota */
            Scalar m_MaxThreadQuotaExcessPercent;
            /* Amount of time a thread should consume per frame */
            Scalar m_ThreadTimeQuota;
            /* Amount of time a thread might consume this frame */
            vector<Scalar> m_ProjectedThreadDurations;
            /* Amount of time a thread did  comsumed last frame */
            vector<Scalar> m_ThreadDurations;
        
            vector<Scalar> m_AverageThreadDifference;
            vector<vector<Scalar>>m_ThreadDifferenceSamples;
            i32 m_ThreadDifferenceSampleCount;
        
            //This vector stays intact while the following one has the elements erased upon usage
            vector<vector<Task*>>m_TaskMemory   [TT_COUNT];
            vector<vector<Task*>>m_Tasks        [TT_COUNT];
            vector<Scalar> m_TaskDurationSamples[TT_COUNT];
            Scalar m_TaskAvgTimes               [TT_COUNT];
            i32 m_DurationSampleCount;
        
            TaskManager* m_TaskManager;
    };
    
    class TaskManager
    {
        public:
            TaskManager();
            ~TaskManager();
        
            void Shutdown();
            void WaitForShutdown();
        
            Time GetRunTime() const { return m_Timer; }
        
            i32 GetCoreCount() const;
            WorkerThread* GetThread(i32 ThreadID) const { return m_Threads[ThreadID]; }
        
            void AddTask(Task* T,bool AllowOnMainThread);
            Task* GetSingleTask(i32 DesiredTaskType = -1,i32 ThreadID = 0);
        
            /* Only call on main thread */
            void WorkUntilDone(i32 DesiredTaskType = -1);
            void PostMainThreadWorkDuration(Scalar Duration) { m_LastMainThreadWorkDuration = Duration; }
            Scalar GetLastMainThreadWorkDuration() const { return m_LastMainThreadWorkDuration; }
        
            void WakeThreads();
            void WaitForThreads() const;
        
            void BeginFrame() { m_TaskContainer.BeginFrame(); WakeThreads(); }
            void EndFrame();
        
            Scalar GetThreadLastFrameDuration(i32 ThreadID) const { return m_ThreadFrameDurations[ThreadID]; }
            Scalar GetAverageTaskDuration(TASK_TYPE Type) const { return m_AverageTaskDurations[Type]; }
        
            bool HasWork(i32 ThreadID) const;
        
            TaskContainer* GetTaskContainer() { return &m_TaskContainer; }
            
        protected:
            friend class WorkerThread;
            Timer m_Timer;
            Scalar m_LastMainThreadWorkDuration;
            Scalar m_AverageTaskDurations[TT_COUNT];
            TaskContainer m_TaskContainer;
            vector<WorkerThread*> m_Threads;
            vector<Time> m_ThreadFrameDurations;
            bool m_IsShuttingDown;
    };
};