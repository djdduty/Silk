#pragma once
#include <Math/Math.h>

#include <string>
#include <vector>
using namespace std;

namespace Silk
{
    class RenderObject;
    class Mesh
    {
        public:
            enum ATTRIB_TYPE
            {
                AT_BYTE,
                AT_UBYTE,
                AT_SHORT,
                AT_USHORT,
                AT_INT,
                AT_UINT,
                AT_FLOAT,
                AT_DOUBLE,
            };
        
            struct MeshAttribute
            {
                string ShaderName;
                i32 ShaderIndex;
                ATTRIB_TYPE Type;
                i32 Size;
                i32 ComponentCount;
                i32 Count;
                i32 Stride;
                void* Pointer;
                bool IsStatic;
            };
        
            Mesh();
        
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            i32 Refs() const { return m_RefCount; }
            void Destroy();
        
            void SetIndexBuffer   (i32 Count,void* Indices  ,bool IsStatic = true,i32 Stride = 0);
            void SetVertexBuffer  (i32 Count,void* Vertices ,bool IsStatic = true,i32 Stride = 0);
            void SetNormalBuffer  (i32 Count,void* Normals  ,bool IsStatic = true,i32 Stride = 0);
            void SetTangentBuffer (i32 Count,void* Tangents ,bool IsStatic = true,i32 Stride = 0);
            void SetColorBuffer   (i32 Count,void* Colors   ,bool IsStatic = true,i32 Stride = 0);
            void SetTexCoordBuffer(i32 Count,void* TexCoords,bool IsStatic = true,i32 Stride = 0);
        
            i32 GetVertexCount() const;
            i32 GetIndexCount() const;
            bool IsIndexed() const { return m_IndexBufferID != -1; }
            i32 GetVisibleInstanceCount() const { return m_VisibleInstanceCount; }
            const vector<RenderObject*>* GetInstanceList() const { return &m_Instances; }
            bool IsInstanced() const { return m_Instances.size() != 0; }
        
            void AddAttribute(string ShaderName,i32 ShaderIndex,ATTRIB_TYPE Type,i32 ComponentCount,i32 Size,i32 Count,i32 Stride,void* Pointer,bool IsStatic = true);
        
            i32 GetAttributeCount() const { return m_Attributes.size(); }
            const MeshAttribute* GetAttribute(i32 Index) const { return &m_Attributes[Index]; }
        
            RenderObject* GetBaseObject() const { return m_Obj; }
        
        protected:
            friend class Scene;
            friend class Renderer;
            friend class ObjectList;
            friend class RenderObject;
            friend class CullingAlgorithm;
            
            ~Mesh();
            RenderObject* m_Obj;
            i32 GetAttributeIndex(i32 ShaderIndex) const;
            i32 m_IndexBufferID;
            i32 m_MeshListID;
            i32 m_RefCount;
            vector<MeshAttribute> m_Attributes;
            vector<RenderObject*> m_Instances;
            i32 m_VisibleInstanceCount;
    };
};

