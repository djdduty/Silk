#pragma once
#include <Math/Math.h>
#include <vector>

using namespace std;
using namespace Silk;
namespace TestClient
{
    class ObjLoader
    {
        public:
            ObjLoader () { }
            ObjLoader (CString Filename) { Load(Filename); }
            ~ObjLoader() { }

            void Load(CString Filename);
            void ComputeTangents();

            i32 GetIndexCount() const { return m_Indices  .size(); }
            i32 GetVertCount () const { return m_Positions.size(); }

            const u32* GetIndices  () { return &m_Indices  [0]  ; }
            const f32* GetPositions() { return &m_Positions[0].x; }
            const f32* GetNormals  () { return &m_Normals  [0].x; }
            const f32* GetTangents () { return &m_Tangents [0].x; }
            const f32* GetTexCoords() { return &m_TexCoords[0].x; }

        private:
            vector<u32 > m_Indices;
            vector<Vec3> m_Positions;
            vector<Vec3> m_Normals;
            vector<Vec3> m_Tangents;
            vector<Vec2> m_TexCoords;
    };
}