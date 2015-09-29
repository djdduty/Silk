#pragma once
#include <Utilities/BoundingBox.h>

#include <vector>
using namespace std;

namespace Silk
{
    class Renderer;
    class Material;
	class Camera;
    class Mesh;
    class DebugDrawer
    {
        public:
            DebugDrawer(Renderer* r);
            virtual ~DebugDrawer();
        
            virtual void Line(const Vec3& a,const Vec3& b,const Vec4& Color);
            virtual void Transform(const Mat4& t,const Vec4& ColorX = Vec4(1,0,0,1),
                                                 const Vec4& ColorY = Vec4(0,1,0,1),
                                                 const Vec4& ColorZ = Vec4(0,0,1,1),
                                                 const Vec3& Scale  = Vec3(1,1,1  ));
            virtual void Box(const Mat4& t,const Vec3& HalfExtents,const Vec4& Color);
            virtual void OBB(const OBB& Box,const Vec4& Color);
            virtual void AABB(const Mat4& ObjTrans,const AABB& Box,const Vec4& Color);
			virtual void DrawCamera(Camera* Cam,const Vec4& Color);
            virtual void DrawMesh(const Mat4& Trans,Mesh* m,const Vec4& Color);
        
            void AddVertex(Scalar x,Scalar y,Scalar z,Scalar r,Scalar g,Scalar b,Scalar a) { AddVertex(Vec3(x,y,z),Vec4(r,g,b,a)); }
            void AddVertex(const Vec3& Vert,const Vec4& Color);
        
            virtual void Update(Scalar dt);
        
            RenderObject* GetObject() const { return m_Display; }
        
        protected:
            Renderer* m_Renderer;
            RenderObject* m_Display;
            Material* m_Material;
        
            vector<Vec3> m_Verts;
            vector<Vec4> m_Colors;
    };
};

