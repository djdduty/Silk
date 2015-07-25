#pragma once
#include <Math/Math.h>

namespace Silk
{
    class Plane
    {
        public:
            Plane() : Offset(0.0f) { }
            Plane(const Vec4& P) : Normal(P.xyz()), Offset(P.w) { }
            Plane(const Vec3& Pt,const Vec3& Normal) { Set(Pt,Normal); }
            ~Plane() { }
        
            void Set(const Vec3& Pt,const Vec3& Normal);
        
            Scalar DistanceToPoint(const Vec3& Pt) const;
            Vec3 GetIntersectionPoint(const Ray& r) const;
        
            void Transform(const Mat4& Trans);
            Plane Transform(const Mat4& Trans) const;
        
            void Normalize();
            Plane Normalized() const;
        
            Vec3 Normal;
            Scalar Offset;
    };
}