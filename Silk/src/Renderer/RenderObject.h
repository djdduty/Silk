#ifndef SILK_RENDER_OBJECT_H
#define SILK_RENDER_OBJECT_H

#include <System/SilkTypes.h>
#include <vector>

#include <Renderer/Mesh.h>
#include <Renderer/Material.h>
#include <Raster/Raster.h>

#include <Math/Math.h>

using namespace std;

namespace Silk {
    class Renderer;
    class ObjectList;

    enum RENDER_OBJECT_TYPE
    {
        ROT_NONE,
        ROT_MESH,
        ROT_CAMERA,
        ROT_LIGHT,
        ROT_COUNT
    };

    class RenderObject {
        public:
            RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObjectIdentifier* ObjectIdentifier);
            ~RenderObject();

            Mesh*       GetMesh()      { return m_Mesh;      }
            Material*   GetMaterial()  { return m_Material;  }
            Mat4        GetTransform() { return m_Transform; }

            void SetMesh(Mesh* M, Material* Mat);
            void SetMaterial(Material* Mat);
            void SetTransform(Mat4 Transform);

        protected:
            friend class Renderer;
            friend class ObjectList;

            RENDER_OBJECT_TYPE      m_Type      ;
            bool                    m_Enabled   ;
            Renderer*               m_Renderer  ;
            RasterObjectIdentifier* m_ObjectIdentifier;
            Material*               m_Material  ;
            Mesh*                   m_Mesh      ;
            Mat4                    m_Transform ;
            //TODO Camera, Light

            //List references
            i32                 m_ListIndex ;
            ObjectList*         m_List      ;

        private:
            void MarkAsUpdated();
    };


    #define ObjectVector vector<RenderObject*>
    class ObjectList {
        public:
            ObjectList() : m_MeshObjects(ObjectVector()), m_LightObjects(ObjectVector()), m_CameraObjects(ObjectVector())
            {};
            ~ObjectList() {}

            i32  AddObject(RenderObject* Obj);
            void RemoveObject(RenderObject* Obj);

            void Clear()
            { 
                m_MeshObjects.clear();
                m_LightObjects.clear();
                m_CameraObjects.clear();
            }

            ObjectVector GetMeshList()   { return m_MeshObjects;   }
            ObjectVector GetCameraList() { return m_CameraObjects; }
            ObjectVector GetLightList()  { return m_LightObjects;  }

        protected:
            friend class Renderer;
            ObjectVector m_MeshObjects;
            ObjectVector m_LightObjects;
            ObjectVector m_CameraObjects;
    };
}

#endif