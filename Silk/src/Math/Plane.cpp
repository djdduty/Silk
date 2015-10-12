#include <Math/Plane.h>

namespace Silk
{
    void Plane::Set(const Vec3& Pt,const Vec3& _Normal)
    {
        Normal = _Normal.Normalized();
        Offset = -Pt.Dot(Normal);
    }
    Scalar Plane::DistanceToPoint(const Vec3& Pt) const
    {
        return Normal.Dot(Pt) + Offset;
    }
    Vec3 Plane::GetIntersectionPoint(const Ray& r) const
    {
        f64 t = -(Normal.Dot(r.Point) + Offset) / Normal.Dot(r.Dir);
        //if(t < 0.0f) return Vec3(0,0,0);
        
        return r.Point + (r.Dir * t);
    }

    void Plane::Transform(const Mat4& Trans)
    {
        Vec3 O = Trans * (Normal * Offset);
        Vec3 N = Trans.Inverse().Transpose() * Normal;
        Set(O,N);
        Normalize();
    }
    Plane Plane::Transform(const Mat4& Trans) const
    {
        Vec3 O = Trans * (Normal * Offset);
        Vec3 N = Trans.Inverse().Transpose() * Normal;
        
        return Plane(O,N).Normalized();
    }
    
    void Plane::Normalize()
    {
        Scalar MagSq = Normal.MagnitudeSq();
        if(MagSq == 0) return;
        
        Scalar iDistance = 1.0f / sqrt(MagSq);
        Normal *= iDistance;
        Offset *= iDistance;
    }
    Plane Plane::Normalized() const
    {
        Scalar MagSq = Normal.MagnitudeSq();
        if(MagSq == 0) return *this;
        
        Scalar iDistance = 1.0f / sqrt(MagSq);
        Vec3 n = Normal * iDistance;
        Scalar o = Offset * iDistance;
        return Plane(Vec4(n,o));
    }
}