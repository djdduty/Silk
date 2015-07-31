#include <Renderer/RenderObject.h>
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>

namespace Silk {
    RenderObject::RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObject* Object) : 
        m_Object(Object), m_Type(Type), m_Enabled(true), m_IsCulled(false), m_Renderer(Renderer), m_Material(0), m_Mesh(0), m_Light(0),
        m_Uniforms(0), m_ShaderListIndex(-1), m_MeshListIndex(-1), m_ListIndex(0), m_List(0), m_InstanceList(0), m_InstanceIndex(-1),
        m_CulledInstanceIndex(-1)
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
        if(m_Renderer) m_Renderer->GetScene()->AddToUpdateList(this);
        m_DidUpdate = true;
    }

    ObjectList::ObjectList(const ObjectList& l)
    {
        m_MeshObjects     = SilkObjectVector(l.m_MeshObjects  );
        m_LightObjects    = SilkObjectVector(l.m_LightObjects );
        m_CameraObjects   = SilkObjectVector(l.m_CameraObjects);
        m_ObjectsByShader = vector<vector<RenderObject*> >(l.m_ObjectsByShader);
        m_ObjectsByMesh   = vector<vector<RenderObject*> >(l.m_ObjectsByMesh  );
        m_ShadersUsed     = vector<Shader*>(l.m_ShadersUsed);
    }
    i32 ObjectList::AddObject(RenderObject* Obj)
    {
        if(m_IsIndexed) if(Obj->m_List && Obj->m_List != this) Obj->m_List->RemoveObject(Obj);
        
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
                    m_ObjectsByShader.push_back(vector<RenderObject*>());
                    ShaderIdx = m_ObjectsByShader.size() - 1;
                }
                
                m_ObjectsByShader[ShaderIdx].push_back(Obj);
                if(m_IsIndexed)
                {
                    Obj->m_ShaderListIndex = m_ObjectsByShader[ShaderIdx].size() - 1;
                
                    Mesh* m = Obj->GetMesh();
                    if(m->m_MeshListID == -1)
                    {
                        m_Meshes.push_back(m);
                        m->m_MeshListID = m_Meshes.size() - 1;
                        
                        m_ObjectsByMesh.push_back(vector<RenderObject*>());
                        m_ObjectsByMesh[m->m_MeshListID].push_back(Obj);
                        Obj->m_MeshListIndex = m_ObjectsByMesh[m->m_MeshListID].size() - 1;
                    }
                }
                
                return m_MeshObjects.size() - 1;
                break;
            }
            case ROT_LIGHT: {
                m_LightObjects.push_back(Obj);
                return m_LightObjects.size() - 1;
                break;
            }
            case ROT_CAMERA: {
                m_CameraObjects.push_back(Obj);
                return m_CameraObjects.size() - 1;
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
                if(m_IsIndexed)
                {
                    if(Obj && Obj->m_List == this) m_MeshObjects.erase(m_MeshObjects.begin()+Obj->m_ListIndex);
                    for(i32 i = Obj->m_ListIndex;i < m_MeshObjects.size();i++) m_MeshObjects[i]->m_ListIndex = i;
                    
                    Mesh* m = Obj->GetMesh();
                    if(m_ObjectsByMesh[m->m_MeshListID].size() == 1)
                    {
                        //Remove mesh from list and remove newly empty object list from m_ObjectsByMesh
                        m_ObjectsByMesh.erase(m_ObjectsByMesh.begin() + m->m_MeshListID);
                        m_Meshes       .erase(m_Meshes       .begin() + m->m_MeshListID);
                        
                        //Renew mesh list indices
                        for(i32 i = m->m_MeshListID;i < m_Meshes.size();i++)
                        {
                            m_Meshes[i]->m_MeshListID = i;
                        }
                    }
                    else
                    {
                        //Remove object from m_ObjectsByMesh
                        m_ObjectsByMesh[m->m_MeshListID].erase(m_ObjectsByMesh[m->m_MeshListID].begin() + Obj->m_MeshListIndex);
                        
                        //Renew mesh list indices
                        for(i32 i = Obj->m_MeshListIndex;i < m_ObjectsByMesh[m->m_MeshListID].size();i++)
                        {
                            m_ObjectsByMesh[m->m_MeshListID][i]->m_MeshListIndex = i;
                        }
                    }
                }
                else
                {
                    for(i32 i = 0;i < m_MeshObjects.size();i++)
                    {
                        if(m_MeshObjects[i] == Obj) m_MeshObjects.erase(m_MeshObjects.begin() + i);
                    }
                }
                break;
            }
            case ROT_LIGHT :
            { 
                if(m_IsIndexed)
                {
                    if(Obj && Obj->m_List == this) m_LightObjects.erase(m_LightObjects.begin()+Obj->m_ListIndex);
                    for(i32 i = Obj->m_ListIndex;i < m_LightObjects.size();i++) m_LightObjects[i]->m_ListIndex = i;
                }
                else
                {
                    for(i32 i = 0;i < m_LightObjects.size();i++)
                    {
                        if(m_LightObjects[i] == Obj) m_LightObjects.erase(m_LightObjects.begin() + i);
                    }
                }
                break;
            }
            case ROT_CAMERA:
            { 
                if(m_IsIndexed)
                {
                    if(Obj && Obj->m_List == this) m_CameraObjects.erase(m_CameraObjects.begin()+Obj->m_ListIndex);
                    for(i32 i = Obj->m_ListIndex;i < m_CameraObjects.size();i++) m_CameraObjects[i]->m_ListIndex = i;
                }
                else
                {
                    for(i32 i = 0;i < m_CameraObjects.size();i++)
                    {
                        if(m_CameraObjects[i] == Obj) m_CameraObjects.erase(m_CameraObjects.begin() + i);
                    }
                }
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
                    if(m_ObjectsByShader[i].size() == 1)
                    {
                        m_ObjectsByShader.erase(m_ObjectsByShader.begin() + i);
                        m_ShadersUsed    .erase(m_ShadersUsed    .begin() + i);
                    }
                    else
                    {
                        if(m_IsIndexed)
                        {
                            m_ObjectsByShader[i].erase(m_ObjectsByShader[i].begin() + Obj->m_ShaderListIndex);
                            for(i32 o = 0;o < m_ObjectsByShader[i].size();o++) m_ObjectsByShader[i][o]->m_ShaderListIndex = o;
                        }
                        else
                        {
                            for(i32 sidx = 0;sidx < m_ObjectsByShader[i].size();i++)
                            {
                                if(m_ObjectsByShader[i][sidx] == Obj)
                                {
                                    m_ObjectsByShader[i].erase(m_ObjectsByShader[i].begin() + sidx);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            
            if(m_IsIndexed) //Only use unindexed lists for temporary storage (eg. culled object lists)
            {
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
        }

        if(m_IsIndexed)
        {
            Obj->m_List = 0;
            Obj->m_ListIndex = 0;
        }
    }
    void ObjectList::Clear()
    {
        m_MeshObjects.clear();
        m_LightObjects.clear();
        m_CameraObjects.clear();
    }
}