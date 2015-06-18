#include <Renderer/Mesh.h>

namespace Silk
{
    Mesh::Mesh()
    {
    }
    Mesh::~Mesh()
    {
    }
    
    void Mesh::SetVertexBuffer(i32 Count,void* Vertices,bool IsStatic,i32 Stride)
    {
        AddAttribute(PositionAttribName,PositionAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Vertices);
    }
    void Mesh::SetNormalBuffer(i32 Count,void* Normals,bool IsStatic,i32 Stride)
    {
        AddAttribute(NormalAttribName,NormalAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Normals);
    }
    void Mesh::SetTangentBuffer(i32 Count,void* Tangents,bool IsStatic,i32 Stride)
    {
        AddAttribute(TangentAttribName,TangentAttribIndex,AT_FLOAT,3,Count * sizeof(f32) * 3,Count,Stride,Tangents);
    }
    void Mesh::SetColorBuffer(i32 Count,void* Colors,bool IsStatic,i32 Stride)
    {
        AddAttribute(ColorAttribName,ColorAttribIndex,AT_FLOAT,4,Count * sizeof(f32) * 4,Count,Stride,Colors);
    }
    void Mesh::SetTexCoordBuffer(i32 Count,void* TexCoords,bool IsStatic,i32 Stride)
    {
        AddAttribute(TexCoordAttribName,TexCoordAttribIndex,AT_FLOAT,2,Count * sizeof(f32) * 2,Count,Stride,TexCoords);
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
        a.Pointer        = Pointer       ;
        a.IsStatic       = IsStatic      ;
        
        m_Attributes.push_back(a);
    }
    
    i32 Mesh::GetAttributeIndex(i32 ShaderIndex) const
    {
        for(i32 i = 0;i < m_Attributes.size();i++) { if(m_Attributes[i].ShaderIndex == ShaderIndex) { return i; } }
        return -1;
    }
};
