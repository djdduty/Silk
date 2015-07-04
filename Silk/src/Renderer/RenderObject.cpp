#include <Renderer/RenderObject.h>
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>

namespace Silk {
    RenderObject::RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObject* Object) : 
        m_Mesh(0), m_Material(0), m_Renderer(Renderer), m_Type(Type), m_Enabled(true), m_List(0), m_ListIndex(0), m_Object(Object),
        m_Light(0)
    {
        m_Uniforms = new ModelUniformSet(m_Renderer,this);
    }

    RenderObject::~RenderObject() 
    {
        m_Object->Destroy(this);
        delete m_Uniforms;
        if(m_Mesh  ) m_Mesh  ->Destroy();
        //if(m_Light ) m_Light ->Destroy();
        //if(m_Camera) m_Camera->Destroy();
    }

    bool RenderObject::IsInstanced()
    {
        return m_Object->IsInstanced();
    }
    void RenderObject::SetMesh(Mesh* M,Material* Mat)
    {
        if(m_Mesh != M && M && Mat && m_Type != ROT_MESH)
        {
            if(M && Mat) ERROR("Could not set the mesh of RenderObject, this object is not a mesh object! (Object: 0x%lX)\n",intptr_t(this));
            else         ERROR("Could not set the mesh of RenderObject, the material and/or the mesh is null! (Object: 0x%lX)\n",intptr_t(this));
            return;
        }

        if((m_Mesh && (m_Mesh != M)) && m_Mesh->m_Obj == this)
        {
            if((*m_InstanceList)[0] != this) m_Mesh->m_Obj = (*m_InstanceList)[0];
            else m_Mesh->m_Obj = (*m_InstanceList)[1];
            
            m_Object->RemoveInstance(m_InstanceIndex);
            for(i32 i = m_InstanceIndex;i < m_InstanceList->size();i++)
            {
                (*m_InstanceList)[i]->m_InstanceIndex--;
            }
            m_InstanceList->pop_back();
        }
        
        m_Mesh = M;
        m_Material = Mat;
        
        if(Mat->GetShader()->SupportsInstancing())
        {
            m_Mesh->m_Instances.push_back(this);
            m_InstanceIndex = m_Mesh->m_Instances.size() - 1;
            m_InstanceList  = &m_Mesh->m_Instances;
            
            if(m_Mesh->Refs() == 1)
            {
                m_Object->SetMesh(m_Mesh);
                m_Mesh->m_Obj = this;
            }
            else
            {
                if(m_Object) m_Object->Destroy(this);
                m_Type = ROT_MESH;
                m_Object = m_Mesh->m_Obj->m_Object;
                m_Object->AddRef();
                m_Object->AddInstance();
                m_Mesh->m_Obj->GetUniformSet()->SetIsInstanced(true);
            }
        }
        else
        {
            m_Object->SetMesh(m_Mesh);
        }
        m_Mesh->AddRef();
        
        MarkAsUpdated();
    }

    void RenderObject::SetMaterial(Material* Mat)
    {
        if(Mat && m_Type != ROT_MESH) {
            if(Mat)
                ERROR("Could not set the material of RenderObject, this object is not a mesh object!\n");
            else
                ERROR("Coult not set the material of RenderObject, the material is null!\n");
            return;
        }
        m_Material = Mat;
    }

    void RenderObject::SetLight(Light* L)
    {
        if(L && m_Type != ROT_LIGHT) {
            if(L)
                ERROR("Could not set the light of RenderObject, this object is not a light object!\n");
            else
                ERROR("Could not set the light of RenderObject, the light is null!\n");
            return;
        }
        
        m_Light = L;
    }

    void RenderObject::SetTransform(Mat4 Transform)
    {
        m_Uniforms->SetModelMatrix(Transform);
        MarkAsUpdated();
    }
    void RenderObject::SetTextureTransform(Mat4 Transform)
    {
        m_Uniforms->SetTextureMatrix(Transform);
    }
    void RenderObject::UpdateUniforms()
    {
        m_Uniforms->UpdateUniforms();
        m_DidUpdate = false;
    }

    void RenderObject::MarkAsUpdated()
    {
        if(m_Renderer) m_Renderer->AddToUpdateList(this);
        m_DidUpdate = true;
    }

    i32 ObjectList::AddObject(RenderObject* Obj)
    {
        if(Obj->m_List && Obj->m_List != this) Obj->m_List->RemoveObject(Obj);
        
        switch(Obj->m_Type) {
            case ROT_MESH: {
                m_MeshObjects.push_back(Obj);
                
                /* Add to the shader-object list */
                Shader* s = Obj->GetMaterial()->GetShader();
                i32 ShaderIdx = -1;
                for(i32 i = 0;i < m_ShadersUsed.size();i++)
                {
                    if(s == m_ShadersUsed[i]) { ShaderIdx = i; break; }
                }
                if(ShaderIdx == -1)
                {
                    m_ShadersUsed.push_back(s);
                    m_ShaderObjects.push_back(vector<RenderObject*>());
                    ShaderIdx = m_ShaderObjects.size() - 1;
                }
                
                m_ShaderObjects[ShaderIdx].push_back(Obj);
                Obj->m_ShaderListIndex = m_ShaderObjects[ShaderIdx].size() - 1;
                
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
        return -1;
    }

    void ObjectList::RemoveObject(RenderObject* Obj)
    {
        switch(Obj->m_Type)
        {
            case ROT_MESH  :
            { 
                if(Obj && Obj->m_List == this) m_MeshObjects.erase(m_MeshObjects.begin()+Obj->m_ListIndex);
                for(i32 i = Obj->m_ListIndex;i < m_MeshObjects.size();i++) m_MeshObjects[i]->m_ListIndex = i;
                break;
            }
            case ROT_LIGHT :
            { 
                if(Obj && Obj->m_List == this) m_LightObjects.erase(m_LightObjects.begin()+Obj->m_ListIndex);
                for(i32 i = Obj->m_ListIndex;i < m_LightObjects.size();i++) m_LightObjects[i]->m_ListIndex = i;
                break;
            }
            case ROT_CAMERA:
            { 
                if(Obj && Obj->m_List == this) m_CameraObjects.erase(m_CameraObjects.begin()+Obj->m_ListIndex);
                for(i32 i = Obj->m_ListIndex;i < m_CameraObjects.size();i++) m_CameraObjects[i]->m_ListIndex = i;
                break;
            }
            default:
            {
                break;
            }
        }
        
        if(Obj->m_Type == ROT_MESH)
        {
            /* Remove it from the shader-object list */
            Shader* s = Obj->GetMaterial()->GetShader();
            for(i32 i = 0;i < m_ShadersUsed.size();i++)
            {
                if(m_ShadersUsed[i] == s)
                {
                    if(m_ShaderObjects[i].size() == 1)
                    {
                        m_ShaderObjects.erase(m_ShaderObjects.begin() + i);
                        m_ShadersUsed  .erase(m_ShadersUsed  .begin() + i);
                    }
                    else
                    {
                        m_ShaderObjects[i].erase(m_ShaderObjects[i].begin() + Obj->m_ShaderListIndex);
                        for(i32 o = 0;o < m_ShaderObjects[i].size();o++) m_ShaderObjects[i][o]->m_ShaderListIndex = o;
                    }
                }
            }
            
            /* Remove it from instance list if necessary */
            if(Obj->m_Object->IsInstanced())
            {
                Obj->m_Object->RemoveInstance(Obj->m_InstanceIndex);
                
                for(i32 i = Obj->m_InstanceIndex;i < Obj->m_Mesh->m_Instances.size() - 1;i++)
                {
                    Obj->m_Mesh->m_Instances[i] = Obj->m_Mesh->m_Instances[i + 1];
                    RenderObject* Instance = Obj->m_Mesh->m_Instances[i];
                    Instance->m_InstanceIndex--;
                }
                Obj->m_Mesh->m_Instances.pop_back();
            }
        }

        Obj->m_List = 0;
        Obj->m_ListIndex = 0;
    }
}