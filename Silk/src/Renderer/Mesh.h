#pragma once
#include <Math/Math.h>

#include <string>
#include <vector>
using namespace std;

namespace Silk
{
    enum PRIMITIVE_TYPE
    {
        PT_POINTS,
        PT_SPRITES,
        PT_LINES,
        PT_LINE_STRIP,
        PT_TRIANGLES,
        PT_TRIANGLE_STRIP,
        PT_TRIANGLE_FAN,
        PT_COUNT,
    };
    
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
            const MeshAttribute* GetIndexAttribute    () const { if(m_IndexBufferID    == -1) { return 0; } return &m_Attributes[m_IndexBufferID   ]; }
            const MeshAttribute* GetVertexAttribute   () const { if(m_VertexBufferID   == -1) { return 0; } return &m_Attributes[m_VertexBufferID  ]; }
            const MeshAttribute* GetNormalAttribute   () const { if(m_NormalBufferID   == -1) { return 0; } return &m_Attributes[m_NormalBufferID  ]; }
            const MeshAttribute* GetTangentAttribute  () const { if(m_TangentBufferID  == -1) { return 0; } return &m_Attributes[m_TangentBufferID ]; }
            const MeshAttribute* GetColorAttribute    () const { if(m_ColorBufferID    == -1) { return 0; } return &m_Attributes[m_ColorBufferID   ]; }
            const MeshAttribute* GetTexCoordAttribute () const { if(m_TexCoordBufferID == -1) { return 0; } return &m_Attributes[m_TexCoordBufferID]; }
        
            RenderObject* GetBaseObject() const { return m_Obj; }
        
            PRIMITIVE_TYPE PrimitiveType;
        
        protected:
            friend class Scene;
            friend class Renderer;
            friend class DeferredRenderer;
            friend class ObjectList;
            friend class RenderObject;
            friend class CullingAlgorithm;
            
            ~Mesh();
            RenderObject* m_Obj;
            i32 GetAttributeIndex(i32 ShaderIndex) const;
            i32 m_IndexBufferID;
            i32 m_VertexBufferID;
            i32 m_NormalBufferID;
            i32 m_TangentBufferID;
            i32 m_ColorBufferID;
            i32 m_TexCoordBufferID;
            i32 m_MeshListID;
            vector<Mesh*>* m_MeshList;
            i32 m_RefCount;
            vector<MeshAttribute> m_Attributes;
            vector<RenderObject*> m_Instances;
            i64 m_LastFrameRendered;
            i32 m_VisibleInstanceCount;
    };
};

