#pragma once
#include <System/SilkTypes.h>

#ifdef _WIN32
#define isnan _isnan
#define isinf !_finite
#elif __APPLE__
#include <cmath>
#endif

#define PI_OVER_180 0.01745329251
#define PI_UNDER_180 57.2957795131
#define PI 3.14159265

#include <string>
using namespace std;

namespace Silk
{
    class Vec2
    {
        public:
            Vec2();
            Vec2(Scalar x,Scalar y);
            ~Vec2() { }
        
            Vec2 operator-() const { return Vec2(-x,-y); }
            Vec2 operator+(Scalar rval) const;
            Vec2 operator-(Scalar rval) const;
            Vec2 operator*(Scalar rval) const;
            Vec2 operator/(Scalar rval) const;
            Vec2 operator+=(Scalar rval);
            Vec2 operator-=(Scalar rval);
            Vec2 operator*=(Scalar rval);
            Vec2 operator/=(Scalar rval);
        
            Vec2 operator+(const Vec2& rval) const;
            Vec2 operator-(const Vec2& rval) const;
            Vec2 operator*(const Vec2& rval) const;
            Vec2 operator/(const Vec2& rval) const;
            Vec2 operator+=(const Vec2& rval);
            Vec2 operator-=(const Vec2& rval);
            Vec2 operator*=(const Vec2& rval);
            Vec2 operator/=(const Vec2& rval);
        
            Scalar Magnitude() const;
            Scalar MagnitudeSq() const;
            void Normalize();
            Vec2 Normalized() const;
            Vec2 Perpendicular() const;
            Scalar Dot(const Vec2& rval) const;
            Scalar Angle() const;
            void ClampMagnitude(Scalar Magnitude);
            Vec2 ClampMagnitude(Scalar Magnitude) const;
            
            Scalar& operator[](i32 cIdx) { return (&x)[cIdx]; }
        
            Scalar x;
            Scalar y;
    };
    
    class Vec3
    {
        public:
            Vec3();
            Vec3(Scalar x,Scalar y,Scalar z);
            Vec3(const Vec2& xy,Scalar z);
            Vec3(Scalar x,const Vec2& yz);
            Vec3(const Vec3& xyz);
            ~Vec3() { }
        
            Vec3& operator=(const Vec3& rval);
        
            Vec3 operator-() const { return Vec3(-x,-y,-z); }
            Vec3 operator+(Scalar rval) const;
            Vec3 operator-(Scalar rval) const;
            Vec3 operator*(Scalar rval) const;
            Vec3 operator/(Scalar rval) const;
            Vec3& operator+=(Scalar rval);
            Vec3& operator-=(Scalar rval);
            Vec3& operator*=(Scalar rval);
            Vec3& operator/=(Scalar rval);
        
            Vec3 operator+(const Vec3& rval) const;
            Vec3 operator-(const Vec3& rval) const;
            Vec3 operator*(const Vec3& rval) const;
            Vec3 operator/(const Vec3& rval) const;
            Vec3& operator+=(const Vec3& rval);
            Vec3& operator-=(const Vec3& rval);
            Vec3& operator*=(const Vec3& rval);
            Vec3& operator/=(const Vec3& rval);
        
            Scalar Magnitude() const;
            Scalar MagnitudeSq() const;
            void Normalize();
            Vec3 Normalized() const;
            Vec3 Cross(const Vec3& rval) const;
            Scalar Dot(const Vec3& rval) const;
            void ClampMagnitude(Scalar Magnitude);
            Vec3 ClampMagnitude(Scalar Magnitude) const;
            Scalar Angle(const Vec3& With) const;
        
            Vec2 xx() const;
            Vec2 xy() const;
            Vec2 xz() const;
            Vec2 yx() const;
            Vec2 yy() const;
            Vec2 yz() const;
            Vec2 zx() const;
            Vec2 zy() const;
            Vec2 zz() const;
        
            Scalar& operator[](i32 cIdx) { return (&x)[cIdx]; }
        
            Scalar x;
            Scalar y;
            Scalar z;
    };
    
    class Vec4
    {
        public:
            Vec4();
            Vec4(Scalar x,Scalar y,Scalar z,Scalar w);
            Vec4(const Vec3& xyz,Scalar w);
            Vec4(Scalar x,const Vec3& yzw);
            ~Vec4() { }
        
            Vec4 operator-() const { return Vec4(-x,-y,-z,-w); }
            Vec4 operator+(Scalar rval) const;
            Vec4 operator-(Scalar rval) const;
            Vec4 operator*(Scalar rval) const;
            Vec4 operator/(Scalar rval) const;
            Vec4 operator+=(Scalar rval);
            Vec4 operator-=(Scalar rval);
            Vec4 operator*=(Scalar rval);
            Vec4 operator/=(Scalar rval);
        
            Vec4 operator+(const Vec4& rval) const;
            Vec4 operator-(const Vec4& rval) const;
            Vec4 operator*(const Vec4& rval) const;
            Vec4 operator/(const Vec4& rval) const;
            Vec4 operator+=(const Vec4& rval);
            Vec4 operator-=(const Vec4& rval);
            Vec4 operator*=(const Vec4& rval);
            Vec4 operator/=(const Vec4& rval);
        
            Scalar Magnitude() const;
            Scalar MagnitudeSq() const;
            void Normalize();
            Vec4 Normalized() const;
            Scalar Dot(const Vec4& rval) const;
            void ClampMagnitude(Scalar Magnitude);
            Vec4 ClampMagnitude(Scalar Magnitude) const;
        
            Scalar& operator[](i32 cIdx) { return (&x)[cIdx]; }
        
            Vec2 xx() const;
            Vec2 xy() const;
            Vec2 xz() const;
            Vec2 xw() const;
            Vec2 yx() const;
            Vec2 yy() const;
            Vec2 yz() const;
            Vec2 yw() const;
            Vec2 zx() const;
            Vec2 zy() const;
            Vec2 zz() const;
            Vec2 zw() const;
            Vec3 xxx() const;
            Vec3 xxy() const;
            Vec3 xxz() const;
            Vec3 xxw() const;
            Vec3 xyx() const;
            Vec3 xyy() const;
            Vec3 xyz() const;
            Vec3 xyw() const;
            Vec3 xzx() const;
            Vec3 xzy() const;
            Vec3 xzz() const;
            Vec3 xzw() const;
            Vec3 xwx() const;
            Vec3 xwy() const;
            Vec3 xwz() const;
            Vec3 xww() const;
            Vec3 yxx() const;
            Vec3 yxy() const;
            Vec3 yxz() const;
            Vec3 yxw() const;
            Vec3 yyx() const;
            Vec3 yyy() const;
            Vec3 yyz() const;
            Vec3 yyw() const;
            Vec3 yzx() const;
            Vec3 yzy() const;
            Vec3 yzz() const;
            Vec3 yzw() const;
            Vec3 ywx() const;
            Vec3 ywy() const;
            Vec3 ywz() const;
            Vec3 yww() const;
            Vec3 zxx() const;
            Vec3 zxy() const;
            Vec3 zxz() const;
            Vec3 zxw() const;
            Vec3 zyx() const;
            Vec3 zyy() const;
            Vec3 zyz() const;
            Vec3 zyw() const;
            Vec3 zzx() const;
            Vec3 zzy() const;
            Vec3 zzz() const;
            Vec3 zzw() const;
            Vec3 zwx() const;
            Vec3 zwy() const;
            Vec3 zwz() const;
            Vec3 zww() const;
            Vec3 wxx() const;
            Vec3 wxy() const;
            Vec3 wxz() const;
            Vec3 wxw() const;
            Vec3 wyx() const;
            Vec3 wyy() const;
            Vec3 wyz() const;
            Vec3 wyw() const;
            Vec3 wzx() const;
            Vec3 wzy() const;
            Vec3 wzz() const;
            Vec3 wzw() const;
            Vec3 wwx() const;
            Vec3 wwy() const;
            Vec3 wwz() const;
            Vec3 www() const;
        
            Scalar x;
            Scalar y;
            Scalar z;
            Scalar w;
    };
    
    class Quat;
    class Mat4
    {
        public:
            Mat4(Scalar i = 1.0);
            Mat4(Scalar xx,Scalar xy,Scalar xz,Scalar xw,
                 Scalar yx,Scalar yy,Scalar yz,Scalar yw,
                 Scalar zx,Scalar zy,Scalar zz,Scalar zw,
                 Scalar wx,Scalar wy,Scalar wz,Scalar ww);
            Mat4(const Mat4& m);
            ~Mat4();
        
            Scalar Determinant() const;
            Mat4 Transpose() const;
            Mat4 Inverse() const;
        
            Vec3 GetTranslation() const;
            Vec3 GetScale      () const;
            Quat GetRotation   () const;
            Vec3 GetAxis(i32 AID) const;
            Vec3 GetX          () const { return Vec3(x.x,y.x,z.x).Normalized(); }
            Vec3 GetY          () const { return Vec3(x.y,y.y,z.y).Normalized(); }
            Vec3 GetZ          () const { return Vec3(x.z,y.z,z.z).Normalized(); }
        
            Mat4 operator*(const Mat4& rVal) const;
            Mat4& operator *=(const Mat4& rVal);
            Vec4 operator*(const Vec4& rVal) const;
            Vec3 operator*(const Vec3& rVal) const;
        
            Vec4& operator[](i32 rIdx)       { return (&x)[rIdx]; }
            Vec4  operator[](i32 rIdx) const { return (&x)[rIdx]; }
        
            Vec4 x;
            Vec4 y;
            Vec4 z;
            Vec4 w;
        
            void DebugPrint() const
            {
                #define NEG_ZERO_FIX(v) ((abs(v) < 0.000001) ? 0.0 : v)
                printf("x: %s%5.6f  %s%5.6f  %s%5.6f  %s%5.6f\n",NEG_ZERO_FIX(x.x) < 0 ? "" : " ",NEG_ZERO_FIX(x.x),
                                                                 NEG_ZERO_FIX(x.y) < 0 ? "" : " ",NEG_ZERO_FIX(x.y),
                                                                 NEG_ZERO_FIX(x.z) < 0 ? "" : " ",NEG_ZERO_FIX(x.z),
                                                                 NEG_ZERO_FIX(x.w) < 0 ? "" : " ",NEG_ZERO_FIX(x.w));
                
                printf("y: %s%5.6f  %s%5.6f  %s%5.6f  %s%5.6f\n",NEG_ZERO_FIX(y.x) < 0 ? "" : " ",NEG_ZERO_FIX(y.x),
                                                                 NEG_ZERO_FIX(y.y) < 0 ? "" : " ",NEG_ZERO_FIX(y.y),
                                                                 NEG_ZERO_FIX(y.z) < 0 ? "" : " ",NEG_ZERO_FIX(y.z),
                                                                 NEG_ZERO_FIX(y.w) < 0 ? "" : " ",NEG_ZERO_FIX(y.w));
                
                printf("z: %s%5.6f  %s%5.6f  %s%5.6f  %s%5.6f\n",NEG_ZERO_FIX(z.x) < 0 ? "" : " ",NEG_ZERO_FIX(z.x),
                                                                 NEG_ZERO_FIX(z.y) < 0 ? "" : " ",NEG_ZERO_FIX(z.y),
                                                                 NEG_ZERO_FIX(z.z) < 0 ? "" : " ",NEG_ZERO_FIX(z.z),
                                                                 NEG_ZERO_FIX(z.w) < 0 ? "" : " ",NEG_ZERO_FIX(z.w));
                
                printf("w: %s%5.6f  %s%5.6f  %s%5.6f  %s%5.6f\n",NEG_ZERO_FIX(w.x) < 0 ? "" : " ",NEG_ZERO_FIX(w.x),
                                                                 NEG_ZERO_FIX(w.y) < 0 ? "" : " ",NEG_ZERO_FIX(w.y),
                                                                 NEG_ZERO_FIX(w.z) < 0 ? "" : " ",NEG_ZERO_FIX(w.z),
                                                                 NEG_ZERO_FIX(w.w) < 0 ? "" : " ",NEG_ZERO_FIX(w.w));
            }
        
            static Mat4 Identity;
    };
    
    Mat4 Scale(const Vec3& ScaleFactors);
    Mat4 Scale(Scalar ScaleFactor);
    Mat4 Translation(const Vec3& Pos);
    Mat4 Rotation(const Vec3& Axis,Scalar Angle,const Vec3& Center);
    Mat4 Rotation(const Vec3& Axis,Scalar Angle);
    Mat4 Rotation(Scalar Yaw,Scalar Pitch,Scalar Roll);
    Mat4 Rotation(const Vec3& Basis1,const Vec3& Basis2,const Vec3& Basis3);
    Mat4 RotationX(Scalar Theta);
    Mat4 RotationY(Scalar Theta);
    Mat4 RotationZ(Scalar Theta);
    //Renamed from "Camera", was conflicting with the Camera class
    Mat4 CreateTransform(const Vec3& Eye,const Vec3& Look,const Vec3& Up,const Vec3& Right);
    Mat4 LookAt(const Vec3& Eye,const Vec3& At,const Vec3& Up);
    Mat4 Orthogonal(Scalar Width,Scalar Height,Scalar Near,Scalar Far);
    Mat4 Orthogonal(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom, Scalar Near, Scalar Far);
    Mat4 Perspective(Scalar Width,Scalar Height,Scalar Near,Scalar Far);
    Mat4 PerspectiveFov(Scalar FOV,Scalar Aspect,Scalar Near,Scalar Far);
    Mat4 PerspectiveMultiFov(Scalar FovX,Scalar FovY,Scalar Near,Scalar Far);
    Mat4 Viewport(Scalar Width,Scalar Height);
    
    class Quat
    {
        public:
            Quat();
            Quat(Scalar Ax,Scalar Ay,Scalar Az,Scalar Angle);
            Quat(const Vec3& Axis,Scalar Angle);
            Quat(const Quat& q);
            ~Quat();
            
            void FromAxisAngle(Scalar Ax,Scalar Ay,Scalar Az,Scalar Angle);
            void FromAxisAngle(const Vec3& Axis,Scalar Angle);
        
            void LookAt(Vec3 From,Vec3 To);
            Mat4 ToMat() const;
            Vec3 ToEuler() const;
        
            Quat operator *(const Quat& rVal) const;
            Vec3 operator *(const Vec3& rVal) const;
            Quat& operator *=(const Quat& rVal);
        
            Scalar x;
            Scalar y;
            Scalar z;
            Scalar w;
    };
    
    class iv2
    {
        public:
            iv2() : x(0), y(0) { }
            iv2(i32 _x,i32 _y) : x(_x), y(_y) { }
            ~iv2() { }
            
            i32 x,y;
    };
    class iv3
    {
        public:
            iv3() : x(0), y(0), z(0) { }
            iv3(i32 _x,i32 _y,i32 _z) : x(_x), y(_y), z(_z) { }
            ~iv3() { }
            
            i32 x,y,z;
    };
    class iv4
    {
        public:
            iv4() : x(0), y(0), z(0), w(0) { }
            iv4(i32 _x,i32 _y,i32 _z,i32 _w) : x(_x), y(_y), z(_z), w(_w) { }
            ~iv4() { }
            
            i32 x,y,z,w;
    };
    
    typedef struct _Ray
    {
        Vec3 Point;
        Vec3 Dir;
    } Ray;

    Ray UnProject(const Vec3& win,const Mat4& v,const Mat4& p,const Vec4& viewport);
    bool ClosestPointsBetweenLines(const Vec3& l0_Point,const Vec3& l0_Offset,const Vec3& l1_Point,const Vec3& l1_Offset,Vec3& Out0,Vec3& Out1);
    bool ClosestPointsBetweenRays(const Ray& r0,const Ray& r1,Vec3& Out0,Vec3& Out1);
    
    Vec2 Abs(const Vec2& v);
    Vec3 Abs(const Vec3& v);
    Vec4 Abs(const Vec4& v);
    
    Vec2 Min(const Vec2& v0,const Vec2& v1);
    Vec3 Min(const Vec3& v0,const Vec3& v1);
    Vec4 Min(const Vec4& v0,const Vec4& v1);
    Vec2 Max(const Vec2& v0,const Vec2& v1);
    Vec3 Max(const Vec3& v0,const Vec3& v1);
    Vec4 Max(const Vec4& v0,const Vec4& v1);
    
    void SeedRandom(i32 Seed);
    Scalar Random(Scalar Min,Scalar Max);
    Scalar Random   (Scalar Span           );
    Vec3   Random   (Vec3 Min   ,Vec3   Max);
    Vec3   RandomVec(Scalar Span           );
    Vec3 ColorFunc(Scalar Factor);
    bool IsPower2(i32 x);
}