#include <Renderer/RenderObject.h>
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>

namespace Silk {
    RenderObject::RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObjectIdentifier* ObjectIdentifier) : 
        m_Mesh(0), m_Material(0), m_Renderer(Renderer), m_Type(Type), m_Enabled(true), m_List(0), m_ListIndex(0), m_ObjectIdentifier(ObjectIdentifier),
        m_Light(0)
    {
        m_Uniforms = new ModelUniformSet(m_Renderer);
        m_Uniforms->Texture = m_Uniforms->Model = m_Uniforms->MV = m_Uniforms->MVP = m_Uniforms->Normal = Mat4::Identity;
    }

    RenderObject::~RenderObject() 
    {
        delete m_ObjectIdentifier;
        delete m_Uniforms;
    }

    void RenderObject::SetMesh(Mesh* M, Material* Mat)
    {
        if(M && Mat && m_Type != ROT_MESH) {
            if(M && Mat)
                ERROR("Could not set the mesh of RenderObject, this object is not a mesh object!\n");
            else
                ERROR("Coult not set the mesh of RenderObject, the material and or the mesh is null!\n");
            return;
        }

        m_Mesh = M;
        m_Material = Mat;
        m_ObjectIdentifier->SetMesh(m_Mesh);
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
        m_Uniforms->Model = Transform;
        m_Uniforms->Normal = m_Uniforms->Model.Inverse().Transpose();
        m_Uniforms->Normal[0][3] = m_Uniforms->Normal[1][3] = m_Uniforms->Normal[2][3] = 0.0f;
        MarkAsUpdated();
    }
    void RenderObject::SetTextureTransform(Mat4 Transform)
    {
        m_Uniforms->Texture = Transform;
    }
    void RenderObject::UpdateUniforms()
    {
        Camera* Cam = m_Renderer->GetActiveCamera();
        if(Cam)
        {
            m_Uniforms->MV     = Cam->GetInversed  () * m_Uniforms->Model ;
            m_Uniforms->MVP    = Cam->GetProjection() * m_Uniforms->MV    ;
        }
        else
        {
            m_Uniforms->MV     = m_Uniforms->MVP = Mat4::Identity       ;
        }

        m_Uniforms->UpdateUniforms();
    }

    void RenderObject::MarkAsUpdated()
    {
        if(m_Renderer)
            m_Renderer->AddToUpdateList(this);
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
        SilkObjectVector TypeVector;
        switch(Obj->m_Type) {
            case ROT_MESH:   { TypeVector = m_MeshObjects  ; break; }
            case ROT_LIGHT:  { TypeVector = m_LightObjects ; break; }
            case ROT_CAMERA: { TypeVector = m_CameraObjects; break; }
            default: {
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
        }

        if(Obj && Obj->m_List == this)
            TypeVector.erase(TypeVector.begin()+Obj->m_ListIndex);

        for(i32 i = Obj->m_ListIndex; i < TypeVector.size(); i++)
            TypeVector[i]->m_ListIndex = i;

        Obj->m_List = 0;
        Obj->m_ListIndex = 0;
    }
}