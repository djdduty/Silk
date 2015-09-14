/*
 *  Parse.h
 *  Silk
 *
 *  Created by Michael DeCicco on 9/13/15.
 *
 */

#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>
#include <string>
using namespace std;

namespace Silk
{
    string ReadNextWord   (char* Data,const string& NonDelimiters,i32 &Offset);
    string ReadNextString (char* Data,i32& Offset);
    i32    ReadNextInt32  (char* Data,i32& Offset);
    i64    ReadNextInt64  (char* Data,i32& Offset);
    f32    ReadNextFloat32(char* Data,i32& Offset);
    f64    ReadNextFloat64(char* Data,i32& Offset);
    Vec2   ReadNextVec2   (char* Data,i32& Offset);
    Vec3   ReadNextVec3   (char* Data,i32& Offset);
    Vec4   ReadNextVec4   (char* Data,i32& Offset);
    Mat4   ReadNextMat4   (char* Data,i32& Offset);
    
    /* Returns false if no delimiter is encountered */
    bool   ReadUntil      (char* Data,const string& Delimiters,i32& Offset);
    bool   ReadUntil      (char* Data,const string& Delimiters,i32& Offset,string& Output);
};

