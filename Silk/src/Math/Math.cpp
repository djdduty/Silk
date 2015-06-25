#include <Math/Math.h>

#include <math.h>
#include <random>
#include <ctime>

namespace Silk
{
    Vec2::Vec2()
    {
    }
    Vec2::Vec2(Scalar _x,Scalar _y) : x(_x), y(_y)
    {
    }
    Vec2 Vec2::operator+(Scalar rval) const
    {
        return Vec2(x + rval,y + rval);
    }
    Vec2 Vec2::operator-(Scalar rval) const
    {
        return Vec2(x - rval,y - rval);
    }
    Vec2 Vec2::operator*(Scalar rval) const
    {
        return Vec2(x * rval,y * rval);
    }
    Vec2 Vec2::operator/(Scalar rval) const
    {
        return Vec2(x / rval,y / rval);
    }
    Vec2 Vec2::operator+=(Scalar rval)
    {
        return Vec2(x += rval,y += rval);
    }
    Vec2 Vec2::operator-=(Scalar rval)
    {
        return Vec2(x -= rval,y -= rval);
    }
    Vec2 Vec2::operator*=(Scalar rval)
    {
        return Vec2(x *= rval,y *= rval);
    }
    Vec2 Vec2::operator/=(Scalar rval)
    {
        return Vec2(x /= rval,y /= rval);
    }
    
    Vec2 Vec2::operator+(const Vec2& rval) const
    {
        return Vec2(x + rval.x,y + rval.y);
    }
    Vec2 Vec2::operator-(const Vec2& rval) const
    {
        return Vec2(x - rval.x,y - rval.y);
    }
    Vec2 Vec2::operator*(const Vec2& rval) const
    {
        return Vec2(x * rval.x,y * rval.y);
    }
    Vec2 Vec2::operator/(const Vec2& rval) const
    {
        return Vec2(x / rval.x,y / rval.y);
    }
    Vec2 Vec2::operator+=(const Vec2& rval)
    {
        return Vec2(x += rval.x,y += rval.y);
    }
    Vec2 Vec2::operator-=(const Vec2& rval)
    {
        return Vec2(x -= rval.x,y -= rval.y);
    }
    Vec2 Vec2::operator*=(const Vec2& rval)
    {
        return Vec2(x *= rval.x,y *= rval.y);
    }
    Vec2 Vec2::operator/=(const Vec2& rval)
    {
        return Vec2(x /= rval.x,y /= rval.y);
    }

    Scalar Vec2::Magnitude() const
    {
        #ifdef USE_DOUBLE_SCALAR
        return sqrt((x * x) + (y * y));
        #else
        return sqrtf((x * x) + (y * y));
        #endif
    }
    Scalar Vec2::MagnitudeSq() const
    {
        return (x * x) + (y * y);
    }
    void Vec2::Normalize()
    {
        Scalar mag = Magnitude();
        x /= mag;
        y /= mag;
    }
    Vec2 Vec2::Normalized() const
    {
        Scalar mag = Magnitude();
        return Vec2(x / mag,y / mag);
    }
    Vec2 Vec2::Perpendicular() const
    {
        return Vec2(-y,x);
    }
    Scalar Vec2::Dot(const Vec2& rval) const
    {
        return (x * rval.x) + (y * rval.y);
    }
    Scalar Vec2::Angle() const
    {
        #ifdef USE_DOUBLE_SCALAR
        Scalar a = atan2(x,y) * 57.2957795131f;
        #else
        Scalar a = atan2(x,y) * 57.2957795131f;
        #endif
        if(a < 0) return a + 360.0f;
        return a;
    }
    void Vec2::ClampMagnitude(Scalar Magnitude)
    {
        Normalize();
        x *= Magnitude;
        y *= Magnitude;
    }
    Vec2 Vec2::ClampMagnitude(Scalar Magnitude) const
    {
        return Normalized() * Magnitude;
    }
    
    Vec3::Vec3() : x(Scalar(0)), y(Scalar(0)), z(Scalar(0))
    {
    }
    Vec3::Vec3(Scalar _x,Scalar _y,Scalar _z) : x(_x), y(_y), z(_z)
    {
    }
    Vec3::Vec3(const Vec2& xy,Scalar _z) : x(xy.x), y(xy.y), z(_z)
    {
    }
    Vec3::Vec3(Scalar _x,const Vec2& yz) : x(_x), y(yz.x), z(yz.y)
    {
    }
    Vec3::Vec3(const Vec3& xyz) : x(xyz.x), y(xyz.y), z(xyz.z)
    {
    }
    Vec3& Vec3::operator=(const Vec3& rval)
    {
        x = rval.x;
        y = rval.y;
        z = rval.z;
        return *this;
    }
    Vec3 Vec3::operator+(Scalar rval) const
    {
        return Vec3(x + rval,y + rval,z + rval);
    }
    Vec3 Vec3::operator-(Scalar rval) const
    {
        return Vec3(x - rval,y - rval,z - rval);
    }
    Vec3 Vec3::operator*(Scalar rval) const
    {
        return Vec3(x * rval,y * rval,z * rval);
    }
    Vec3 Vec3::operator/(Scalar rval) const
    {
        return Vec3(x / rval,y / rval,z / rval);
    }
    Vec3& Vec3::operator+=(Scalar rval)
    {
        x += rval;
        y += rval;
        z += rval;
        return *this;
    }
    Vec3& Vec3::operator-=(Scalar rval)
    {
        x -= rval;
        y -= rval;
        z -= rval;
        return *this;
    }
    Vec3& Vec3::operator*=(Scalar rval)
    {
        x *= rval;
        y *= rval;
        z *= rval;
        return *this;
    }
    Vec3& Vec3::operator/=(Scalar rval)
    {
        x *= rval;
        y *= rval;
        z *= rval;
        return *this;
    }
    
    Vec3 Vec3::operator+(const Vec3& rval) const
    {
        return Vec3(x + rval.x,y + rval.y,z + rval.z);
    }
    Vec3 Vec3::operator-(const Vec3& rval) const
    {
        return Vec3(x - rval.x,y - rval.y,z - rval.z);
    }
    Vec3 Vec3::operator*(const Vec3& rval) const
    {
        return Vec3(x * rval.x,y * rval.y,z * rval.z);
    }
    Vec3 Vec3::operator/(const Vec3& rval) const
    {
        return Vec3(x / rval.x,y / rval.y,z / rval.z);
    }
    Vec3& Vec3::operator+=(const Vec3& rval)
    {
        x += rval.x;
        y += rval.y;
        z += rval.z;
        return *this;
    }
    Vec3& Vec3::operator-=(const Vec3& rval)
    {
        x -= rval.x;
        y -= rval.y;
        z -= rval.z;
        return *this;
    }
    Vec3& Vec3::operator*=(const Vec3& rval)
    {
        x *= rval.x;
        y *= rval.y;
        z *= rval.z;
        return *this;
    }
    Vec3& Vec3::operator/=(const Vec3& rval)
    {
        x *= rval.x;
        y *= rval.y;
        z *= rval.z;
        return *this;
    }

    Scalar Vec3::Magnitude() const
    {
        #ifdef USE_DOUBLE_SCALAR
        return sqrt((x * x) + (y * y) + (z * z));
        #else
        return sqrtf((x * x) + (y * y) + (z * z));
        #endif
    }
    Scalar Vec3::MagnitudeSq() const
    {
        return (x * x) + (y * y) + (z * z);
    }
    void Vec3::Normalize()
    {
        Scalar mag = Magnitude();
        if(mag == 0.0f) return;
        mag = 1.0f / mag;
        x *= mag;
        y *= mag;
        z *= mag;
    }

    Vec3 Vec3::Normalized() const
    {
        Scalar mag = Magnitude();
        if(mag == 0.0f) return Vec3(0,0,0);
        mag = 1.0f / mag;
        return Vec3(x * mag,y * mag,z * mag);
    }

    Vec3 Vec3::Cross(const Vec3& rval) const
    {
        return Vec3(y * rval.z - z * rval.y, z * rval.x - x * rval.z, x * rval.y - y * rval.x);
    }
    Scalar Vec3::Dot(const Vec3& rval) const
    {
        return (x * rval.x) + (y * rval.y) + (z * rval.z);
    }
    void Vec3::ClampMagnitude(Scalar Magnitude)
    {
        Normalize();
        x *= Magnitude;
        y *= Magnitude;
        z *= Magnitude;
    }
    Vec3 Vec3::ClampMagnitude(Scalar Magnitude) const
    {
        return Normalized() * Magnitude;
    }
    Scalar Vec3::Angle(const Vec3 &With) const
    {
        Scalar LeftLength = Magnitude();
        Scalar RightLength = With.Magnitude();
        if(LeftLength > 0.0f && RightLength > 0.0f)
        {
            Scalar v = Dot(With) / LeftLength / RightLength;
            if(v < -1.0f) v = 1.0f;
            if(v > 1.0f) v = 1.0f;
            return acosf(v);
        }
        return 0.0f;
    }

    Vec2 Vec3::xx() const { return Vec2(x,x); }
    Vec2 Vec3::xy() const { return Vec2(x,y); }
    Vec2 Vec3::xz() const { return Vec2(x,z); }
    Vec2 Vec3::yx() const { return Vec2(y,x); }
    Vec2 Vec3::yy() const { return Vec2(y,y); }
    Vec2 Vec3::yz() const { return Vec2(y,z); }
    Vec2 Vec3::zx() const { return Vec2(z,x); }
    Vec2 Vec3::zy() const { return Vec2(z,y); }
    Vec2 Vec3::zz() const { return Vec2(z,z); }
    
    Vec4::Vec4() : x(Scalar(0)), y(Scalar(0)), z(Scalar(0)), w(Scalar(0))
    {
    }
    Vec4::Vec4(Scalar _x,Scalar _y,Scalar _z,Scalar _w) : x(_x), y(_y), z(_z), w(_w)
    {
    }
    Vec4::Vec4(const Vec3& xyz,Scalar _w) : x(xyz.x), y(xyz.y), z(xyz.z), w(_w)
    {
    }
    Vec4::Vec4(Scalar _x,const Vec3& yzw) : x(_x), y(yzw.x), z(yzw.y), w(yzw.z)
    {
    }
    Vec4 Vec4::operator+(Scalar rval) const
    {
        return Vec4(x + rval,y + rval,z + rval,w + rval);
    }
    Vec4 Vec4::operator-(Scalar rval) const
    {
        return Vec4(x - rval,y - rval,z - rval,w - rval);
    }
    Vec4 Vec4::operator*(Scalar rval) const
    {
        return Vec4(x * rval,y * rval,z * rval,w * rval);
    }
    Vec4 Vec4::operator/(Scalar rval) const
    {
        return Vec4(x / rval,y / rval,z / rval,w / rval);
    }
    Vec4 Vec4::operator+=(Scalar rval)
    {
        return Vec4(x += rval,y += rval,z += rval,w += rval);
    }
    Vec4 Vec4::operator-=(Scalar rval)
    {
        return Vec4(x -= rval,y -= rval,z -= rval,w -= rval);
    }
    Vec4 Vec4::operator*=(Scalar rval)
    {
        return Vec4(x *= rval,y *= rval,z *= rval,w *= rval);
    }
    Vec4 Vec4::operator/=(Scalar rval)
    {
        return Vec4(x /= rval,y /= rval,z /= rval,w /= rval);
    }
    
    Vec4 Vec4::operator+(const Vec4& rval) const
    {
        return Vec4(x + rval.x,y + rval.y,z + rval.z,w + rval.w);
    }
    Vec4 Vec4::operator-(const Vec4& rval) const
    {
        return Vec4(x - rval.x,y - rval.y,z - rval.z,w - rval.w);
    }
    Vec4 Vec4::operator*(const Vec4& rval) const
    {
        return Vec4(x * rval.x,y * rval.y,z * rval.z,w * rval.w);
    }
    Vec4 Vec4::operator/(const Vec4& rval) const
    {
        return Vec4(x / rval.x,y / rval.y,z / rval.z,w / rval.w);
    }
    Vec4 Vec4::operator+=(const Vec4& rval)
    {
        return Vec4(x += rval.x,y += rval.y,z += rval.z,w += rval.w);
    }
    Vec4 Vec4::operator-=(const Vec4& rval)
    {
        return Vec4(x -= rval.x,y -= rval.y,z -= rval.z,w -= rval.w);
    }
    Vec4 Vec4::operator*=(const Vec4& rval)
    {
        return Vec4(x *= rval.x,y *= rval.y,z *= rval.z,w *= rval.w);
    }
    Vec4 Vec4::operator/=(const Vec4& rval)
    {
        return Vec4(x /= rval.x,y /= rval.y,z /= rval.z,w /= rval.w);
    }

    Scalar Vec4::Magnitude() const
    {
        #ifdef USE_DOUBLE_SCALAR
        return sqrt((x * x) + (y * y) + (z * z) + (w * w));
        #else
        return sqrtf((x * x) + (y * y) + (z * z) + (w * w));
        #endif
    }
    Scalar Vec4::MagnitudeSq() const
    {
        return (x * x) + (y * y) + (z * z) + (w * w);
    }
    void Vec4::Normalize()
    {
        Scalar mag = Magnitude();
        x /= mag;
        y /= mag;
        z /= mag;
        w /= mag;
    }
    Vec4 Vec4::Normalized() const
    {
        Scalar mag = Magnitude();
        return Vec4(x / mag,y / mag,z / mag,w / mag);
    }
    Scalar Vec4::Dot(const Vec4& rval) const
    {
        return (x * rval.x) + (y * rval.y) + (z * rval.z) + (w * rval.w);
    }
    void Vec4::ClampMagnitude(Scalar Magnitude)
    {
        Normalize();
        x *= Magnitude;
        y *= Magnitude;
        z *= Magnitude;
        w *= Magnitude;
    }
    Vec4 Vec4::ClampMagnitude(Scalar Magnitude) const
    {
        return Normalized() * Magnitude;
    }
    Vec2 Vec4::xx() const { return Vec2(x,x); }
    Vec2 Vec4::xy() const { return Vec2(x,y); }
    Vec2 Vec4::xz() const { return Vec2(x,z); }
    Vec2 Vec4::xw() const { return Vec2(x,w); }
    Vec2 Vec4::yx() const { return Vec2(y,x); }
    Vec2 Vec4::yy() const { return Vec2(y,y); }
    Vec2 Vec4::yz() const { return Vec2(y,z); }
    Vec2 Vec4::yw() const { return Vec2(y,w); }
    Vec2 Vec4::zx() const { return Vec2(z,x); }
    Vec2 Vec4::zy() const { return Vec2(z,y); }
    Vec2 Vec4::zz() const { return Vec2(z,z); }
    Vec2 Vec4::zw() const { return Vec2(z,w); }
    Vec3 Vec4::xxx() const { return Vec3(x,x,x); }
    Vec3 Vec4::xxy() const { return Vec3(x,x,y); }
    Vec3 Vec4::xxz() const { return Vec3(x,x,z); }
    Vec3 Vec4::xxw() const { return Vec3(x,x,w); }
    Vec3 Vec4::xyx() const { return Vec3(x,y,x); }
    Vec3 Vec4::xyy() const { return Vec3(x,y,y); }
    Vec3 Vec4::xyz() const { return Vec3(x,y,z); }
    Vec3 Vec4::xyw() const { return Vec3(x,y,w); }
    Vec3 Vec4::xzx() const { return Vec3(x,z,x); }
    Vec3 Vec4::xzy() const { return Vec3(x,z,y); }
    Vec3 Vec4::xzz() const { return Vec3(x,z,z); }
    Vec3 Vec4::xzw() const { return Vec3(x,z,w); }
    Vec3 Vec4::xwx() const { return Vec3(x,w,x); }
    Vec3 Vec4::xwy() const { return Vec3(x,w,y); }
    Vec3 Vec4::xwz() const { return Vec3(x,w,z); }
    Vec3 Vec4::xww() const { return Vec3(x,w,w); }
    Vec3 Vec4::yxx() const { return Vec3(y,x,x); }
    Vec3 Vec4::yxy() const { return Vec3(y,x,y); }
    Vec3 Vec4::yxz() const { return Vec3(y,x,z); }
    Vec3 Vec4::yxw() const { return Vec3(y,x,w); }
    Vec3 Vec4::yyx() const { return Vec3(y,y,x); }
    Vec3 Vec4::yyy() const { return Vec3(y,y,y); }
    Vec3 Vec4::yyz() const { return Vec3(y,y,z); }
    Vec3 Vec4::yyw() const { return Vec3(y,y,w); }
    Vec3 Vec4::yzx() const { return Vec3(y,z,x); }
    Vec3 Vec4::yzy() const { return Vec3(y,z,y); }
    Vec3 Vec4::yzz() const { return Vec3(y,z,z); }
    Vec3 Vec4::yzw() const { return Vec3(y,z,w); }
    Vec3 Vec4::ywx() const { return Vec3(y,w,x); }
    Vec3 Vec4::ywy() const { return Vec3(y,w,y); }
    Vec3 Vec4::ywz() const { return Vec3(y,w,z); }
    Vec3 Vec4::yww() const { return Vec3(y,w,w); }
    Vec3 Vec4::zxx() const { return Vec3(z,x,x); }
    Vec3 Vec4::zxy() const { return Vec3(z,x,y); }
    Vec3 Vec4::zxz() const { return Vec3(z,x,z); }
    Vec3 Vec4::zxw() const { return Vec3(z,x,w); }
    Vec3 Vec4::zyx() const { return Vec3(z,y,x); }
    Vec3 Vec4::zyy() const { return Vec3(z,y,y); }
    Vec3 Vec4::zyz() const { return Vec3(z,y,z); }
    Vec3 Vec4::zyw() const { return Vec3(z,y,w); }
    Vec3 Vec4::zzx() const { return Vec3(z,z,x); }
    Vec3 Vec4::zzy() const { return Vec3(z,z,y); }
    Vec3 Vec4::zzz() const { return Vec3(z,z,z); }
    Vec3 Vec4::zzw() const { return Vec3(z,z,w); }
    Vec3 Vec4::zwx() const { return Vec3(z,w,x); }
    Vec3 Vec4::zwy() const { return Vec3(z,w,y); }
    Vec3 Vec4::zwz() const { return Vec3(z,w,z); }
    Vec3 Vec4::zww() const { return Vec3(z,w,w); }
    Vec3 Vec4::wxx() const { return Vec3(w,x,x); }
    Vec3 Vec4::wxy() const { return Vec3(w,x,y); }
    Vec3 Vec4::wxz() const { return Vec3(w,x,z); }
    Vec3 Vec4::wxw() const { return Vec3(w,x,w); }
    Vec3 Vec4::wyx() const { return Vec3(w,y,x); }
    Vec3 Vec4::wyy() const { return Vec3(w,y,y); }
    Vec3 Vec4::wyz() const { return Vec3(w,y,z); }
    Vec3 Vec4::wyw() const { return Vec3(w,y,w); }
    Vec3 Vec4::wzx() const { return Vec3(w,z,x); }
    Vec3 Vec4::wzy() const { return Vec3(w,z,y); }
    Vec3 Vec4::wzz() const { return Vec3(w,z,z); }
    Vec3 Vec4::wzw() const { return Vec3(w,z,w); }
    Vec3 Vec4::wwx() const { return Vec3(w,w,x); }
    Vec3 Vec4::wwy() const { return Vec3(w,w,y); }
    Vec3 Vec4::wwz() const { return Vec3(w,w,z); }
    Vec3 Vec4::www() const { return Vec3(w,w,w); }
    
    Vec3 Quat::operator *(const Vec3& rVal) const
    {
        Vec3 p0(x,y,z);
        Vec3 t = p0.Cross(rVal) * 2.0f;
        Vec3 v1 = rVal + (t * w) + p0.Cross(t);
        return v1;
    }
    Mat4::Mat4()
    {
        *this = Identity;
    }
    Mat4::Mat4(Scalar xx,Scalar xy,Scalar xz,Scalar xw,
               Scalar yx,Scalar yy,Scalar yz,Scalar yw,
               Scalar zx,Scalar zy,Scalar zz,Scalar zw,
               Scalar wx,Scalar wy,Scalar wz,Scalar ww)
    {
        x = Vec4(xx,xy,xz,xw);
        y = Vec4(yx,yy,yz,yw);
        z = Vec4(zx,zy,zz,zw);
        w = Vec4(wx,wy,wz,ww);
    }
    Mat4::Mat4(const Mat4& m)
    {
        memcpy(&x,&m.x,sizeof(Vec4) * 4);
    }
    Mat4::~Mat4()
    {
    }
    Scalar Mat4::Determinant() const
    {
        Scalar Result[4][4];
        Scalar tmp[12]; /* temp array for pairs */
        Scalar src[16]; /* array of transpose source Mat */
        
        for(u32 i = 0; i < 4; i++)
        {
            src[i + 0 ] = (*this)[i][0];
            src[i + 4 ] = (*this)[i][1];
            src[i + 8 ] = (*this)[i][2];
            src[i + 12] = (*this)[i][3];
        }
        
        tmp[0 ] = src[10] * src[15];
        tmp[1 ] = src[11] * src[14];
        tmp[2 ] = src[9 ] * src[15];
        tmp[3 ] = src[11] * src[13];
        tmp[4 ] = src[9 ] * src[14];
        tmp[5 ] = src[10] * src[13];
        tmp[6 ] = src[8 ] * src[15];
        tmp[7 ] = src[11] * src[12];
        tmp[8 ] = src[8 ] * src[14];
        tmp[9 ] = src[10] * src[12];
        tmp[10] = src[8 ] * src[13];
        tmp[11] = src[9 ] * src[12];
        
        Result[0][0]  = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4 ] * src[7];
        Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5 ] * src[7];
        Result[0][1]  = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9 ] * src[7];
        Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8 ] * src[7];
        Result[0][2]  = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
        Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
        Result[0][3]  = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
        Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
        Result[1][0]  = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5 ] * src[3];
        Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4 ] * src[3];
        Result[1][1]  = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8 ] * src[3];
        Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9 ] * src[3];
        Result[1][2]  = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
        Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
        Result[1][3]  = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
        Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
        
        tmp[0 ] = src[2] * src[7];
        tmp[1 ] = src[3] * src[6];
        tmp[2 ] = src[1] * src[7];
        tmp[3 ] = src[3] * src[5];
        tmp[4 ] = src[1] * src[6];
        tmp[5 ] = src[2] * src[5];

        tmp[6 ] = src[0] * src[7];
        tmp[7 ] = src[3] * src[4];
        tmp[8 ] = src[0] * src[6];
        tmp[9 ] = src[2] * src[4];
        tmp[10] = src[0] * src[5];
        tmp[11] = src[1] * src[4];
        
        Result[2][0]  = tmp[0 ] * src[13] + tmp[3 ] * src[14] + tmp[4 ] * src[15];
        Result[2][0] -= tmp[1 ] * src[13] + tmp[2 ] * src[14] + tmp[5 ] * src[15];
        Result[2][1]  = tmp[1 ] * src[12] + tmp[6 ] * src[14] + tmp[9 ] * src[15];
        Result[2][1] -= tmp[0 ] * src[12] + tmp[7 ] * src[14] + tmp[8 ] * src[15];
        Result[2][2]  = tmp[2 ] * src[12] + tmp[7 ] * src[13] + tmp[10] * src[15];
        Result[2][2] -= tmp[3 ] * src[12] + tmp[6 ] * src[13] + tmp[11] * src[15];
        Result[2][3]  = tmp[5 ] * src[12] + tmp[8 ] * src[13] + tmp[11] * src[14];
        Result[2][3] -= tmp[4 ] * src[12] + tmp[9 ] * src[13] + tmp[10] * src[14];
        Result[3][0]  = tmp[2 ] * src[10] + tmp[5 ] * src[11] + tmp[1 ] * src[9 ];
        Result[3][0] -= tmp[4 ] * src[11] + tmp[0 ] * src[9 ] + tmp[3 ] * src[10];
        Result[3][1]  = tmp[8 ] * src[11] + tmp[0 ] * src[8 ] + tmp[7 ] * src[10];
        Result[3][1] -= tmp[6 ] * src[10] + tmp[9 ] * src[11] + tmp[1 ] * src[8 ];
        Result[3][2]  = tmp[6 ] * src[9 ] + tmp[11] * src[11] + tmp[3 ] * src[8 ];
        Result[3][2] -= tmp[10] * src[11] + tmp[2 ] * src[8 ] + tmp[7 ] * src[9 ];
        Result[3][3]  = tmp[10] * src[10] + tmp[4 ] * src[8 ] + tmp[9 ] * src[9 ];
        Result[3][3] -= tmp[8 ] * src[9 ] + tmp[11] * src[10] + tmp[5 ] * src[8 ];
        
        return 1.0f / (src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3]);
    }
    Mat4 Mat4::Inverse() const
    {
        Scalar Result[4][4];
        Scalar tmp[12]; /* temp array for pairs */
        Scalar src[16]; /* array of transpose source Mat */
        Scalar det;     /* determinant */
        
        for(u32 i = 0; i < 4; i++)
        {
            src[i + 0 ] = (*this)[i][0];
            src[i + 4 ] = (*this)[i][1];
            src[i + 8 ] = (*this)[i][2];
            src[i + 12] = (*this)[i][3];
        }
        
        tmp[0 ] = src[10] * src[15];
        tmp[1 ] = src[11] * src[14];
        tmp[2 ] = src[9 ] * src[15];
        tmp[3 ] = src[11] * src[13];
        tmp[4 ] = src[9 ] * src[14];
        tmp[5 ] = src[10] * src[13];
        tmp[6 ] = src[8 ] * src[15];
        tmp[7 ] = src[11] * src[12];
        tmp[8 ] = src[8 ] * src[14];
        tmp[9 ] = src[10] * src[12];
        tmp[10] = src[8 ] * src[13];
        tmp[11] = src[9 ] * src[12];
        
        Result[0][0]  = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4 ] * src[7];
        Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5 ] * src[7];
        Result[0][1]  = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9 ] * src[7];
        Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8 ] * src[7];
        Result[0][2]  = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
        Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
        Result[0][3]  = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
        Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
        Result[1][0]  = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5 ] * src[3];
        Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4 ] * src[3];
        Result[1][1]  = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8 ] * src[3];
        Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9 ] * src[3];
        Result[1][2]  = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
        Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
        Result[1][3]  = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
        Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
        
        tmp[0 ] = src[2] * src[7];
        tmp[1 ] = src[3] * src[6];
        tmp[2 ] = src[1] * src[7];
        tmp[3 ] = src[3] * src[5];
        tmp[4 ] = src[1] * src[6];
        tmp[5 ] = src[2] * src[5];

        tmp[6 ] = src[0] * src[7];
        tmp[7 ] = src[3] * src[4];
        tmp[8 ] = src[0] * src[6];
        tmp[9 ] = src[2] * src[4];
        tmp[10] = src[0] * src[5];
        tmp[11] = src[1] * src[4];
        
        Result[2][0]  = tmp[0 ] * src[13] + tmp[3 ] * src[14] + tmp[4 ] * src[15];
        Result[2][0] -= tmp[1 ] * src[13] + tmp[2 ] * src[14] + tmp[5 ] * src[15];
        Result[2][1]  = tmp[1 ] * src[12] + tmp[6 ] * src[14] + tmp[9 ] * src[15];
        Result[2][1] -= tmp[0 ] * src[12] + tmp[7 ] * src[14] + tmp[8 ] * src[15];
        Result[2][2]  = tmp[2 ] * src[12] + tmp[7 ] * src[13] + tmp[10] * src[15];
        Result[2][2] -= tmp[3 ] * src[12] + tmp[6 ] * src[13] + tmp[11] * src[15];
        Result[2][3]  = tmp[5 ] * src[12] + tmp[8 ] * src[13] + tmp[11] * src[14];
        Result[2][3] -= tmp[4 ] * src[12] + tmp[9 ] * src[13] + tmp[10] * src[14];
        Result[3][0]  = tmp[2 ] * src[10] + tmp[5 ] * src[11] + tmp[1 ] * src[9 ];
        Result[3][0] -= tmp[4 ] * src[11] + tmp[0 ] * src[9 ] + tmp[3 ] * src[10];
        Result[3][1]  = tmp[8 ] * src[11] + tmp[0 ] * src[8 ] + tmp[7 ] * src[10];
        Result[3][1] -= tmp[6 ] * src[10] + tmp[9 ] * src[11] + tmp[1 ] * src[8 ];
        Result[3][2]  = tmp[6 ] * src[9 ] + tmp[11] * src[11] + tmp[3 ] * src[8 ];
        Result[3][2] -= tmp[10] * src[11] + tmp[2 ] * src[8 ] + tmp[7 ] * src[9 ];
        Result[3][3]  = tmp[10] * src[10] + tmp[4 ] * src[8 ] + tmp[9 ] * src[9 ];
        Result[3][3] -= tmp[8 ] * src[9 ] + tmp[11] * src[10] + tmp[5 ] * src[8 ];
        
        det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
        
        det = 1.0f / det;

        Mat4 r;
        for(u32 i = 0; i < 4; i++)
        {
            for(u32 j = 0; j < 4; j++)
            {
                r[i][j] = Scalar(Result[i][j] * det);
            }
        }
        return r;
    }

    Mat4 Mat4::Transpose() const
    {
        Mat4 Result;
        for(u32 i = 0;i < 4;i++)
        {
            for(u32 j = 0;j < 4;j++)
            {
                Result[j][i] = (*this)[i][j];
            }
        }
        return Result;
    }

    Mat4 Mat4::operator*(const Mat4& rVal) const
    {
        Mat4 r;
        for(i32 i = 0; i < 4; i++)
        {
            for(i32 j = 0;j < 4;j++)
            {
                for(i32 k = 0;k < 4;k++)
                {
                    r[i][j] += (*this)[i][k] * rVal[k][j];
                }
            }
        }
        return r;
    }
    Mat4& Mat4::operator *=(const Mat4& rVal)
    {
        Mat4 r;
        for(i32 i = 0; i < 4; i++)
        {
            for(i32 j = 0;j < 4;j++)
            {
                for(i32 k = 0;k < 4;k++)
                {
                    r[i][j] += (*this)[i][k] * rVal[k][j];
                }
            }
        }
        return *this = r;
    }
    Vec4 Mat4::operator*(const Vec4& rVal) const
    {
        Scalar W = rVal.x * x.w +
                   rVal.y * y.w +
                   rVal.z * z.w +
                   rVal.w * w.w;
        if(W)
        {
            const Scalar invW = 1.0f / W;
            return Vec4((rVal.x * x.x + rVal.y * y.x + rVal.z * z.x + w.x) * invW,
                        (rVal.x * x.y + rVal.y * y.y + rVal.z * z.y + w.y) * invW,
                        (rVal.x * x.z + rVal.y * y.z + rVal.z * z.z + w.z) * invW,
                        (rVal.x * x.w + rVal.y * y.w + rVal.z * z.w + w.w) * invW);
        }
        return Vec4();
    }
    Vec3 Mat4::operator*(const Vec3& rVal) const
    {
        Scalar W = rVal.x * x.w +
                   rVal.y * y.w +
                   rVal.z * z.w + w.w;
        if(W)
        {
            const Scalar invW = 1.0f / W;
            return Vec3((rVal.x * x.x + rVal.y * y.x + rVal.z * z.x + w.x) * invW,
                        (rVal.x * x.y + rVal.y * y.y + rVal.z * z.y + w.y) * invW,
                        (rVal.x * x.z + rVal.y * y.z + rVal.z * z.z + w.z) * invW);
        }
        return Vec3();
    }
    Mat4 Rotation(const Vec3& _Basis1, const Vec3& _Basis2, const Vec3& _Basis3)
    {
        Vec3 Basis1 = _Basis1.Normalized();
        Vec3 Basis2 = _Basis2.Normalized();
        Vec3 Basis3 = _Basis3.Normalized();

        Mat4 Result;
        Result[0][0] = Basis1.x;
        Result[1][0] = Basis1.y;
        Result[2][0] = Basis1.z;
        Result[3][0] = 0.0f;

        Result[0][1] = Basis2.x;
        Result[1][1] = Basis2.y;
        Result[2][1] = Basis2.z;
        Result[3][1] = 0.0f;

        Result[0][2] = Basis3.x;
        Result[1][2] = Basis3.y;
        Result[2][2] = Basis3.z;
        Result[3][2] = 0.0f;

        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }

    Mat4 CreateTransform(const Vec3& Eye,const Vec3& _Look,const Vec3& _Up,const Vec3& _Right)
    {
        Vec3 Look = _Look.Normalized();
        Vec3 Up = _Up.Normalized();
        Vec3 Right = _Right.Normalized();

        Mat4 Result;
        Result[0][0] = Right.x;
        Result[1][0] = Right.y;
        Result[2][0] = Right.z;
        Result[3][0] = -Right.Dot(Eye);

        Result[0][1] = Up.x;
        Result[1][1] = Up.y;
        Result[2][1] = Up.z;
        Result[3][1] = -Up.Dot(Eye);

        Result[0][2] = Look.x;
        Result[1][2] = Look.y;
        Result[2][2] = Look.z;
        Result[3][2] = -Look.Dot(Eye);

        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }

    Mat4 LookAt(const Vec3& Eye, const Vec3& At, const Vec3& Up)
    {
        Vec3 XAxis, YAxis, ZAxis;
        ZAxis = (Eye - At);
        XAxis = Up.Cross(ZAxis).Normalized();
        YAxis = ZAxis.Cross(XAxis).Normalized();

        Mat4 Result;
        Result[0][0] = XAxis.x;
        Result[1][0] = XAxis.y;
        Result[2][0] = XAxis.z;
        Result[3][0] = -XAxis.Dot(Eye);

        Result[0][1] = YAxis.x;
        Result[1][1] = YAxis.y;
        Result[2][1] = YAxis.z;
        Result[3][1] = -YAxis.Dot(Eye);

        Result[0][2] = ZAxis.x;
        Result[1][2] = ZAxis.y;
        Result[2][2] = ZAxis.z;
        Result[3][2] = -ZAxis.Dot(Eye);

        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }

    Mat4 Orthogonal(Scalar Width, Scalar Height, Scalar Near, Scalar Far)
    {
        Mat4 Result;
        Result[0][0] = 2.0f / Width;
        Result[1][0] = 0.0f;
        Result[2][0] = 0.0f;
        Result[3][0] = 0.0f;

        Result[0][1] = 0.0f;
        Result[1][1] = 2.0f / Height;
        Result[2][1] = 0.0f;
        Result[3][1] = 0.0f;

        Result[0][2] = 0.0f;
        Result[1][2] = 0.0f;
        Result[2][2] = 1.0f / (Near - Far);
        Result[3][2] = Near / (Near - Far);

        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }
    Mat4 Orthogonal(Scalar Left,Scalar Right,Scalar Top,Scalar Bottom, Scalar Near, Scalar Far)
    {
        Scalar Width = Right - Left;
        Scalar Height = Top - Bottom;
        Scalar Length = Far - Near  ;
        Mat4 Result;
        Result[0][0] = 2.0f / Width;
        Result[1][0] = 0.0f;
        Result[2][0] = 0.0f;
        Result[3][0] = 0.0f;

        Result[0][1] = 0.0f;
        Result[1][1] = 2.0f / Height;
        Result[2][1] = 0.0f;
        Result[3][1] = 0.0f;

        Result[0][2] = 0.0f;
        Result[1][2] = 0.0f;
        Result[2][2] = -2.0f / Length;
        Result[3][2] = Near / (Near - Far);

        Result[0][3] = -((Right + Left) / Width );
        Result[1][3] = -((Top + Bottom) / Height);
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }

    Mat4 Perspective(Scalar Width, Scalar Height, Scalar Near, Scalar Far)
    {
        Mat4 Result;
        Result.x.x = 2.0f * Near / Width;
        Result.y.y = 2.0f * Near / Height;
        Result.z.z = Far / (Near - Far);
        Result.w.z = Far * Near / (Near - Far);
        Result.z.w = -1.0f;
        return Result;
    }

    Mat4 PerspectiveFov(Scalar FOV, Scalar Aspect, Scalar Near, Scalar Far)
    {
        Scalar NearmFar = Near - Far;
        Scalar tFov = 1.0f / tanf(PI_OVER_180 * FOV * 0.5f);
        return Mat4(
            tFov / Aspect, 0   , 0                      , 0,
            0            , tFov, 0                      , 0,
            0            , 0   , (Far + Near) / NearmFar,2.0f * Far * Near / NearmFar,
            0            , 0   ,-1                      , 0);
    }

    Mat4 PerspectiveMultiFov(Scalar FovX, Scalar FovY, Scalar Near, Scalar Far)
    {
        Scalar NearmFar = Near - Far;
        return Mat4(
            1.0f / tanf(PI_OVER_180 * FovX * 0.5f), 0, 0, 0,
            0, 1.0f / tanf(PI_OVER_180 * FovY * 0.5f), 0, 0,
            0, 0, (Far + Near) / NearmFar               ,2.0f * Far * Near / NearmFar,
            0, 0, -1                                    , 0);
    }

    Mat4 Rotation(const Vec3& Axis, Scalar Angle)
    {
        Scalar c = cosf(Angle);
        Scalar s = sinf(Angle);
        Scalar t = 1.0f - c;

        Vec3 NormalizedAxis = Axis.Normalized();
        Scalar x = NormalizedAxis.x;
        Scalar y = NormalizedAxis.y;
        Scalar z = NormalizedAxis.z;

        Mat4 Result;
        Result[0][0] = 1 + t*(x*x-1);
        Result[0][1] = z*s+t*x*y;
        Result[0][2] = -y*s+t*x*z;
        Result[0][3] = 0.0f;

        Result[1][0] = -z*s+t*x*y;
        Result[1][1] = 1+t*(y*y-1);
        Result[1][2] = x*s+t*y*z;
        Result[1][3] = 0.0f;

        Result[2][0] = y*s+t*x*z;
        Result[2][1] = -x*s+t*y*z;
        Result[2][2] = 1+t*(z*z-1);
        Result[2][3] = 0.0f;

        Result[3][0] = 0.0f;
        Result[3][1] = 0.0f;
        Result[3][2] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }

    Mat4 Rotation(Scalar Yaw, Scalar Pitch, Scalar Roll)
    {
        return RotationY(Yaw) * RotationX(Pitch) * RotationZ(Roll);
    }

    Mat4 Rotation(const Vec3& Axis, Scalar Angle, const Vec3& Center)
    {
        return Translation(-Center) * Rotation(Axis, Angle) * Translation(Center);
    }

    Mat4 RotationX(Scalar Theta)
    {
        Scalar rad = Theta * PI_OVER_180;
        Scalar CosT = cosf(rad);
        Scalar SinT = sinf(rad);

        Mat4 Result = Mat4::Identity;
        Result[1][1] = CosT;
        Result[1][2] = SinT;
        Result[2][1] = -SinT;
        Result[2][2] = CosT;
        return Result;
    }

    Mat4 RotationY(Scalar Theta)
    {
        Scalar rad = Theta * PI_OVER_180;
        Scalar CosT = cosf(rad);
        Scalar SinT = sinf(rad);

        Mat4 Result = Mat4::Identity;
        Result[0][0] = CosT;
        Result[0][2] = SinT;
        Result[2][0] = -SinT;
        Result[2][2] = CosT;
        return Result;
    }

    Mat4 RotationZ(Scalar Theta)
    {
        Scalar rad = Theta * PI_OVER_180;
        Scalar CosT = cosf(rad);
        Scalar SinT = sinf(rad);

        Mat4 Result = Mat4::Identity;
        Result[0][0] = CosT;
        Result[0][1] = SinT;
        Result[1][0] = -SinT;
        Result[1][1] = CosT;
        return Result;
    }

    Mat4 Scale(const Vec3& ScaleFactors)
    {
        Mat4 Result;
        Result[0][0] = ScaleFactors.x;
        Result[1][0] = 0.0f;
        Result[2][0] = 0.0f;
        Result[3][0] = 0.0f;

        Result[0][1] = 0.0f;
        Result[1][1] = ScaleFactors.y;
        Result[2][1] = 0.0f;
        Result[3][1] = 0.0f;

        Result[0][2] = 0.0f;
        Result[1][2] = 0.0f;
        Result[2][2] = ScaleFactors.z;
        Result[3][2] = 0.0f;

        Result[0][3] = 0.0f;
        Result[1][3] = 0.0f;
        Result[2][3] = 0.0f;
        Result[3][3] = 1.0f;
        return Result;
    }
    Mat4 Scale(Scalar s)
    {
        return Scale(Vec3(s,s,s));
    }
    Mat4 Translation(const Vec3& Pos)
    {
        Mat4 Result = Mat4::Identity;
        Result.x.w = Pos.x;
        Result.y.w = Pos.y;
        Result.z.w = Pos.z;
        return Result;
    }

    Mat4 Viewport(Scalar Width,Scalar Height)
    {
        return Scale(Vec3(Width * 0.5f,-Height * 0.5f,1.0f)) * Translation(Vec3(Width * 0.5f,Height * 0.5f,0.0f));
    }
    Mat4 Mat4::Identity = Mat4(1,0,0,0,
                                        0,1,0,0,
                                        0,0,1,0,
                                        0,0,0,1);
    Quat::Quat() : x(0), y(0), z(0), w(0)
    {
    }
    Quat::Quat(Scalar Ax,Scalar Ay,Scalar Az,Scalar Angle)
    {
        Scalar a = (Angle * PI_OVER_180) * 0.5f;
        Scalar r = Scalar(sin(a));
        w = Scalar(cos(a));
        x = Ax * r;
        y = Ay * r;
        z = Az * r;
    }
    Quat::Quat(const Vec3& Axis,Scalar Angle)
    {
        Scalar a = (Angle * PI_OVER_180) * 0.5f;
        Scalar r = Scalar(sin(a));
        w = Scalar(cos(a));
        x = Axis.x * r;
        y = Axis.y * r;
        z = Axis.z * r;
    }
    Quat::Quat(const Quat& q)
    {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
    }
    Quat::~Quat()
    {
    }

    void Quat::LookAt(Vec3 From,Vec3 To)
    {
        Vec3 Fwd = (To - From).Normalized();

        Scalar dot = Vec3(0,0,1).Dot(Fwd);

        if(abs(dot - (-1.0f)) < 0.000001f)
        {
            FromAxisAngle(0,1,0,360.0f);
        }
        if(abs(dot - (1.0f)) < 0.000001f)
        {
            FromAxisAngle(0,1,0,0);
        }
        FromAxisAngle(Vec3(0,0,1).Cross(Fwd).Normalized(),acos(dot) * PI_UNDER_180);
    }
    void Quat::FromAxisAngle(Scalar Ax,Scalar Ay,Scalar Az,Scalar Angle)
    {
        Scalar a = (Angle * PI_OVER_180) * 0.5f;
        Scalar r = Scalar(sin(a));
        w = Scalar(cos(a));
        x = Ax * r;
        y = Ay * r;
        z = Az * r;
    }
    void Quat::FromAxisAngle(const Vec3& Axis,Scalar Angle)
    {
        Scalar a = (Angle * PI_OVER_180) * 0.5f;
        Scalar r = Scalar(sin(a));
        w = Scalar(cos(a));
        x = Axis.x * r;
        y = Axis.y * r;
        z = Axis.z * r;
    }
    Mat4 Quat::ToMat() const
    {
        Mat4 r;
        r.x.x = 1.0f - 2.0f * (y * y + z * z);
	    r.x.y = 2.0f * (x * y - w * z);
	    r.x.z = 2.0f * (x * z + w * y);
	    r.x.w = 0.0f;
	    r.y.x = 2.0f * (x * y + w * z);
	    r.y.y = 1.0f - 2.0f * (x * x + z * z);
	    r.y.z = 2.0f * (y * z - w * x);
	    r.y.w = 0.0f;
	    r.z.x = 2.0f * (x * z - w * y);
	    r.z.y = 2.0f * (y * z + w * x);
	    r.z.z = 1.0f - 2.0f * (x * x + y * y);
	    r.z.w = 0.0f;
	    r.w.x = r.w.y = r.w.z = 0;
	    r.w.w = 1.0f;
        return r;
    }

    Quat Quat::operator *(const Quat& q) const
    {
        Quat r;

        r.w = w * q.w - x * q.x - y * q.y - z * q.z;
        r.x = w * q.x + x * q.w + y * q.z - z * q.y;
        r.y = w * q.y + y * q.w + z * q.x - x * q.z;
        r.z = w * q.z + z * q.w + x * q.y - y * q.x;
	
        return r;
    }
    Quat& Quat::operator *=(const Quat& q)
    {
        Quat r;
        r.w = w * q.w - x * q.x - y * q.y - z * q.z;
        r.x = w * q.x + x * q.w + y * q.z - z * q.y;
        r.y = w * q.y + y * q.w + z * q.x - x * q.z;
        r.z = w * q.z + z * q.w + x * q.y - y * q.x;
        
        x = r.x;
        y = r.y;
        z = r.z;
        w = r.w;
        
        return *this;
    }
    
    static std::minstd_rand0 gen;
    void SeedRandom(i32 Seed) { if(Seed == -1) { gen.seed((i32)time(0)); } else gen.seed(Seed); }
    Scalar Random(Scalar Min,Scalar Max) { uniform_real_distribution<> dist(Min,Max); return dist(gen);  }
    Vec3 ColorFunc(Scalar Factor)
    {
        Scalar Div = (abs(fmodf(Factor,1.0f)) * 6.0f);
        Scalar Ascending = fmodf(Div,1.0f);
        Scalar Descending = 1.0f - Ascending;

        switch(int(Div))
        {
            case 0:  { return Vec3(1.0f,Ascending,0.0f);  }
            case 1:  { return Vec3(Descending,1.0f,0.0f); }
            case 2:  { return Vec3(0.0f,1.0f,Ascending);  }
            case 3:  { return Vec3(0.0f,Descending,1.0f); }
            case 4:  { return Vec3(Ascending,0.0f,1.0f);  }
            default: { return Vec3(1.0f,0.0f,Descending); }
        }
        
        return Vec3(1.0f,1.0f,1.0f);
    }
    bool IsPower2(i32 x)
    {
        return (x == 0) ? false : (x & (x - 1)) == 0;
    }
}