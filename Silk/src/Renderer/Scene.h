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
    class CullingAlgorithm;
    
    class Scene
    {
        public:
            Scene(Renderer* r);
            ~Scene();
        
            void SetActiveCamera(Camera* c) { m_ActiveCamera = c;    }
            Camera* GetActiveCamera() const { return m_ActiveCamera; }
        
            void SetCullingAlgorithm(CullingAlgorithm* Algo) { m_CullingAlgorithm = Algo; }
            CullingAlgorithm* GetCullingAlgorithm() const { return m_CullingAlgorithm; }
        
            CullingResult* PerformCulling();

            void AddRenderObject(RenderObject* Object);
            void RemoveRenderObject(RenderObject* Object);
            void AddToUpdateList(RenderObject* Object) { m_UpdatedObjects.push_back(Object); }
        
            ObjectList* GetObjectList() const { return m_ObjectList; }

        protected:
            friend class CullingAlgorithm;
            friend class Renderer;
            
            Renderer* m_Renderer;
            Camera* m_ActiveCamera;
            CullingAlgorithm* m_CullingAlgorithm;

            ObjectList* m_ObjectList;
            vector<RenderObject*> m_UpdatedObjects;
    };
};

