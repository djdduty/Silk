#pragma once
#include <vector>
#include <System/SilkTypes.h>

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
        
        protected:
            Scene* m_Scene;
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
    };
};

