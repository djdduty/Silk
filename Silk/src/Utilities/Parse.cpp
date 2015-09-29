/*
 *  Parse.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 9/13/15.
 *
 */

#ifndef __APPLE__
#define isnumber isdigit
#endif

#include "Parse.h"

#ifdef _WIN32
#define isnumber isdigit
#endif

namespace Silk
{
    string ReadNextWord   (char* Data,const string& NonDelimiters,i32 &Offset)
    {
        string Word;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(isalpha(c)) Word += c;
            else if(Word.length() != 0)
            {
                bool IsNonDelim = false;
                for(i32 i = 0;i < NonDelimiters.length();i++)
                {
                    if(NonDelimiters[i] == c) { IsNonDelim = true; break; }
                }
                if(IsNonDelim) Word += c;
                else { Offset--; break; }
            }
        }
        
        return Word;
    }
    string ReadNextString (char* Data,i32 &Offset)
    {
        string Str;
        while(Data[Offset] != 0)
        {
            if(Data[Offset++] == '\"')
            {
                while(Data[Offset] != 0)
                {
                    char c = Data[Offset++];
                    if(c == '\"') return Str;
                    Str += c;
                }
                ERROR("Expected '\"' to end text string.\n");
                break;
            }
        }
        
        return Str;
    }
    i32    ReadNextInt32  (char* Data,i32 &Offset)
    {
        i32 DecimalCount = 0;
        bool IsNeg = false;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(c == '-') IsNeg = true;
            if(isnumber(c))
            {
                string Val;
                Val += c;
                while(Data[Offset] != 0)
                {
                    c = Data[Offset++];
                    if(isnumber(c) || c == '.')
                    {
                        if(c == '.' && DecimalCount == 1)
                        {
                            ERROR("Unexpected '.' in integer.\n");
                            return (IsNeg ? -1 : 1) * atoi(Val.c_str());
                        }
                        else if(c == '.') DecimalCount++;
                        
                        Val += c;
                    }
                    else { Offset--; break; }
                }
                return (IsNeg ? -1 : 1) * atoi(Val.c_str());
            }
            
        }
        return 0;
    }
    i64    ReadNextInt64  (char* Data,i32 &Offset)
    {
        i32 DecimalCount = 0;
        bool IsNeg = false;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(c == '-') IsNeg = true;
            if(isnumber(c))
            {
                string Val;
                Val += c;
                while(Data[Offset] != 0)
                {
                    c = Data[Offset++];
                    if(isnumber(c) || c == '.')
                    {
                        if(c == '.' && DecimalCount == 1)
                        {
                            ERROR("Unexpected '.' in integer.\n");
                            return (IsNeg ? -1 : 1) * atol(Val.c_str());;
                        }
                        else if(c == '.') DecimalCount++;
                        
                        Val += c;
                    }
                    else { Offset--; break; }
                }
                return (IsNeg ? -1 : 1) * atol(Val.c_str());;
            }
            
        }
        return 0;
    }
    f32    ReadNextFloat32(char* Data,i32 &Offset)
    {
        i32 DecimalCount = 0;
        bool IsNeg = false;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(c == '-') IsNeg = true;
            if(isnumber(c))
            {
                string Val;
                Val += c;
                while(Data[Offset] != 0)
                {
                    c = Data[Offset++];
                    if(isnumber(c) || c == '.')
                    {
                        if(c == '.' && DecimalCount == 2)
                        {
                            ERROR("Unexpected '.' in float32.\n");
                            return (IsNeg ? -1 : 1) * atof(Val.c_str());
                        }
                        else if(c == '.') DecimalCount++;
                        
                        Val += c;
                    }
                    else { Offset--; break; }
                }
                return (IsNeg ? -1 : 1) * atof(Val.c_str());
            }
            
        }
        return 0;
    }
    f64    ReadNextFloat64(char* Data,i32 &Offset)
    {
        i32 DecimalCount = 0;
        bool IsNeg = false;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(c == '-') IsNeg = true;
            if(isnumber(c))
            {
                string Val;
                Val += c;
                while(Data[Offset] != 0)
                {
                    c = Data[Offset++];
                    if(isnumber(c) || c == '.')
                    {
                        if(c == '.' && DecimalCount == 2)
                        {
                            ERROR("Unexpected '.' in float64.\n");
                            return (IsNeg ? -1 : 1) * atof(Val.c_str());
                        }
                        else if(c == '.') DecimalCount++;
                        
                        Val += c;
                    }
                    else { Offset--; break; }
                }
                return (IsNeg ? -1 : 1) * atof(Val.c_str());
            }
            
        }
        return 0;
    }
    Vec2   ReadNextVec2   (char* Data,i32 &Offset)
    {
        Vec2 v;
        v.x = ReadNextFloat32(Data,Offset);
        v.y = ReadNextFloat32(Data,Offset);
        return v;
    }
    Vec3   ReadNextVec3   (char* Data,i32 &Offset)
    {
        Vec3 v;
        v.x = ReadNextFloat32(Data,Offset);
        v.y = ReadNextFloat32(Data,Offset);
        v.z = ReadNextFloat32(Data,Offset);
        return v;
    }
    Vec4   ReadNextVec4   (char* Data,i32 &Offset)
    {
        Vec4 v;
        v.x = ReadNextFloat32(Data,Offset);
        v.y = ReadNextFloat32(Data,Offset);
        v.z = ReadNextFloat32(Data,Offset);
        v.w = ReadNextFloat32(Data,Offset);
        return v;
    }
    Mat4   ReadNextMat4   (char* Data,i32 &Offset)
    {
        Mat4 m;
        m.x = ReadNextVec4(Data,Offset);
        m.y = ReadNextVec4(Data,Offset);
        m.z = ReadNextVec4(Data,Offset);
        m.w = ReadNextVec4(Data,Offset);
        return m;
    }
    bool   ReadUntil      (char* Data,const string& Delimiters,i32& Offset)
    {
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            for(i32 i = 0;i < Delimiters.length();i++)
            {
                if(c == Delimiters[i]) return true;
            }
        }
        return false;
    }
    bool   ReadUntil      (char* Data,const string& Delimiters,i32& Offset,string& Output)
    {
        i32 BrackCount = 0;
        i32 ParenCount = 0;
        i32 BraceCount = 0;
        while(Data[Offset] != 0)
        {
            char c = Data[Offset++];
            if(c == '{') BraceCount++;
            if(c == '[') BrackCount++;
            if(c == '(') ParenCount++;
            
            for(i32 i = 0;i < Delimiters.length();i++)
            {
                if(c == Delimiters[i])
                {
                    if(Delimiters[i] == '}' && BraceCount != 0) { BraceCount--; continue; }
                    if(Delimiters[i] == ']' && BrackCount != 0) { BrackCount--; continue; }
                    if(Delimiters[i] == ')' && ParenCount != 0) { ParenCount--; continue; }
                    return true;
                }
            }
            Output += c;
        }
        return false;
    }
};
