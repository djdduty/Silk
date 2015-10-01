#include <Renderer/Mesh.h>
#include <Renderer/ShaderSystem.h>

namespace Silk
{
    Mesh::Mesh() : PrimitiveType(PT_COUNT), m_IndexBufferID(-1), m_VertexBufferID(-1), m_NormalBufferID(-1), m_TangentBufferID(-1),
                   m_ColorBufferID(-1), m_TexCoordBufferID(-1), m_MeshListID(-1), m_MeshList(0), m_RefCount(1), m_LastFrameRendered(-1),
                   m_VisibleInstanceCount(0)
    {
    }
    Mesh::~Mesh()
    {
        for(i32 i = 0;i < m_Attributes.size();i++)
        {
            if(m_Attributes[i].Pointer) delete [] ((Byte*)m_Attributes[i].Pointer);
        }
        m_Attributes.clear();
    }
    
    void Mesh::Destroy()
    {
        m_RefCount--;
        if(m_RefCount == 0)
        {
            if(m_MeshList)
            {
                m_MeshList->erase(m_MeshList->begin() + m_MeshListID);
                
                for(i32 i = m_MeshListID;i < m_MeshList->size();i++)
                {
                    (*m_MeshList)[i]->m_MeshListID--;
                }
            }
            delete this;
        }
    }
    
    void Mesh::SetIndexBuffer(i32 Count,void* Indices,bool IsStatic,i32 Stride)
    {
        AddAttribute("",-1,AT_UINT,1,Count * sizeof(u32),Count,Stride,Indices);
        m_IndexBufferID = m_Attributes.size() - 1;
    }
    void Mesh::SetVertexBuffer(i32 Count,void* Vertices,bool IsStatic,i32 Stride)
    {
        AddAttribute(PositionAttribName,PositionAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Vertices);
        m_VertexBufferID = m_Attributes.size() - 1;
    }
    void Mesh::SetNormalBuffer(i32 Count,void* Normals,bool IsStatic,i32 Stride)
    {
        AddAttribute(NormalAttribName,NormalAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Normals);
        m_NormalBufferID = m_Attributes.size() - 1;
    }
    void Mesh::SetTangentBuffer(i32 Count,void* Tangents,bool IsStatic,i32 Stride)
    {
        AddAttribute(TangentAttribName,TangentAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Tangents);
        m_TangentBufferID = m_Attributes.size() - 1;
    }
    void Mesh::SetColorBuffer(i32 Count,void* Colors,bool IsStatic,i32 Stride)
    {
        AddAttribute(ColorAttribName,ColorAttribIndex,AT_FLOAT,4,Count * sizeof(f32) * 4,Count,Stride,Colors);
        m_ColorBufferID = m_Attributes.size() - 1;
    }
    void Mesh::SetTexCoordBuffer(i32 Count,void* TexCoords,bool IsStatic,i32 Stride)
    {
        AddAttribute(TexCoordAttribName,TexCoordAttribIndex,AT_FLOAT,2,Count * sizeof(f32) * 2,Count,Stride,TexCoords);
        m_TexCoordBufferID = m_Attributes.size() - 1;
    }
    i32 Mesh::GetVertexCount() const
    {
        //Optimize: store Idx for the position attribute
        i32 Idx = GetAttributeIndex(PositionAttribIndex);
        if(Idx != -1) return m_Attributes[Idx].Count;
        return -1;
    }
    i32 Mesh::GetIndexCount() const
    {
        if(m_IndexBufferID != -1) return m_Attributes[m_IndexBufferID].Count;
        else return -1;
    }
    void Mesh::AddAttribute(string ShaderName,i32 ShaderIndex,ATTRIB_TYPE Type,i32 ComponentCount,i32 Size,i32 Count,i32 Stride,void *Pointer,bool IsStatic)
    {
        if(GetAttributeIndex(ShaderIndex) != -1)
        {
            ERROR("Mesh already contains an attribute buffer for attribute \"%s\". (Mesh: 0x%lX)\n",ShaderName.c_str(),(intptr_t)this);
            return;
        }
        
        MeshAttribute a;
        a.ShaderName     = ShaderName    ;
        a.ShaderIndex    = ShaderIndex   ;
        a.Type           = Type          ;
        a.Size           = Size          ;
        a.ComponentCount = ComponentCount;
        a.Count          = Count         ;
        a.Stride         = Stride        ;
        a.Pointer        = new Byte[Size];
        memcpy(a.Pointer,Pointer,Size);
        a.IsStatic       = IsStatic      ;
        
        m_Attributes.push_back(a);
        
        if(ShaderIndex == -1) m_IndexBufferID = m_Attributes.size() - 1;
    }
    
    i32 Mesh::GetAttributeIndex(i32 ShaderIndex) const
    {
        for(i32 i = 0;i < m_Attributes.size();i++) { if(m_Attributes[i].ShaderIndex == ShaderIndex) { return i; } }
        return -1;
    }
};
