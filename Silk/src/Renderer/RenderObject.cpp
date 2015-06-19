#include <Renderer/RenderObject.h>
#include <Renderer/Renderer.h>

namespace Silk {
    RenderObject::RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObjectIdentifier* ObjectIdentifier) : 
        m_Mesh(0), m_Material(0), m_Renderer(Renderer), m_Type(Type), m_Transform(Mat4()), m_Enabled(true), m_List(0), m_ListIndex(0), m_ObjectIdentifier(ObjectIdentifier)
    {
    }

    RenderObject::~RenderObject() 
    {
        delete m_ObjectIdentifier;
    }

    void RenderObject::SetMesh(Mesh* M, Material* Mat)
    {
        if(m_Type != ROT_MESH) {
            ERROR("Could not set the mesh of RenderObject, this object is not a mesh object!\n");
            return;
        }

        m_Mesh = M;
        m_Material = Mat;
        m_ObjectIdentifier->SetMesh(m_Mesh);
        MarkAsUpdated();
    }

    void RenderObject::SetMaterial(Material* Mat)
    {
        if(m_Type != ROT_MESH) {
            ERROR("Could not set the material of RenderObject, this object is not a mesh object!\n");
            return;
        }
        m_Material = Mat;
    }

    void RenderObject::SetTransform(Mat4 Transform)
    {
        m_Transform = Transform;
        MarkAsUpdated();
    }

    void RenderObject::MarkAsUpdated()
    {
        if(m_Renderer)
            m_Renderer->AddToUpdateList(this);
    }

    i32 ObjectList::AddObject(RenderObject* Obj)
    {
        switch(Obj->m_Type) {
            case ROT_MESH: {
                m_MeshObjects.push_back(Obj);
                return m_MeshObjects.size()-1;
                break;
            }
            case ROT_LIGHT: {
                m_LightObjects.push_back(Obj);
                return m_LightObjects.size()-1;
                break;
            }
            case ROT_CAMERA: {
                m_CameraObjects.push_back(Obj);
                return m_CameraObjects.size()-1;
                break;
            }
            default: {
                break;
            }
        }
    }

    void ObjectList::RemoveObject(RenderObject* Obj)
    {
        ObjectVector TypeVector;
        switch(Obj->m_Type) {
            case ROT_MESH:   { TypeVector = m_MeshObjects  ; break; }
            case ROT_LIGHT:  { TypeVector = m_LightObjects ; break; }
            case ROT_CAMERA: { TypeVector = m_CameraObjects; break; }
            default: {
                break;
            }
        }

        if(Obj && Obj->m_List == this)
            TypeVector.erase(TypeVector.begin()+Obj->m_ListIndex);

        for(i32 i = Obj->m_ListIndex; i < TypeVector.size(); i++)
            TypeVector[i]->m_ListIndex = i;

        Obj->m_List = 0;
        Obj->m_ListIndex = 0;
    }
}