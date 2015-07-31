#pragma once
#include <vector>
#include <System/SilkTypes.h>
#include <System/Threading.h>

using namespace std;
namespace Silk
{
    class Scene;
    class Camera;
    class Material;
    class Renderer;
    class ObjectList;
    class RenderObject;
    class CullingResult;
    
    class CullingAlgorithm
    {
        public:
            CullingAlgorithm(Scene* S) : m_Scene(S) { }
            virtual ~CullingAlgorithm() { }
        
            virtual CullingResult* PerformCulling() = 0;
            virtual const char* GetAlgorithmName() const = 0;
        
            void SetObjectVisibility(RenderObject* Obj,bool Visibility);
        
            Scene* GetScene() const { return m_Scene; }
        
        protected:
            Scene* m_Scene;
            vector<RenderObject*> m_VisibleObjects;
            Mutex m_SetVisibilityMutex;
    };
    
    class CullingResult
    {
        public:
            CullingResult();
            ~CullingResult();
        
            ObjectList* m_VisibleObjects;
        
            /* Actual duration of the culling operation (milliseconds) */
            f32 m_RealDuration;
            /* Average task duration (milliseconds) */
            f32 m_AverageTaskDuration;
            /* Total duration of each task */
            f32 m_TotalTaskDuration;
            /* Efficiency (How many times faster culling is in parallel than in serial, 0 = no measure available) */
            f32 m_Efficiency;
    };
};

