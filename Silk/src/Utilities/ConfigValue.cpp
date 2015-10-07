#include <Utilities/ConfigurationManager.h>
#include <Utilities/Utilities.h>

namespace Silk
{
    const char* vtStrings[ConfigValue::VT_COUNT] =
    {
        "Byte",
        "uByte",
        "string",
        "bool",
        "i16",
        "u16",
        "i32",
        "u32",
        "i64",
        "u64",
        "f32",
        "f64",
        "Vec2",
        "Vec3",
        "Vec4",
        "Mat4"
    };
    
    ConfigValue::ConfigValue(VALUE_TYPE Type,void* Ptr) : m_TargetValue(Ptr), m_Type(Type), m_DidChange(true)
    {
        switch(m_Type)
        {
            case VT_BYTE  : { m_TypeSize = sizeof(Byte  ); Set(*( Byte *)Ptr); break; }
            case VT_UBYTE : { m_TypeSize = sizeof(uByte ); Set(*(uByte *)Ptr); break; }
            case VT_STRING: { m_TypeSize = sizeof(string); Set(*(string*)Ptr); break; }
            case VT_BOOL  : { m_TypeSize = sizeof(bool  ); Set(*(bool  *)Ptr); break; }
            case VT_I16   : { m_TypeSize = sizeof(i16   ); Set(*(i16   *)Ptr); break; }
            case VT_U16   : { m_TypeSize = sizeof(u16   ); Set(*(u16   *)Ptr); break; }
            case VT_I32   : { m_TypeSize = sizeof(i32   ); Set(*(i32   *)Ptr); break; }
            case VT_U32   : { m_TypeSize = sizeof(u32   ); Set(*(u32   *)Ptr); break; }
            case VT_I64   : { m_TypeSize = sizeof(i64   ); Set(*(i64   *)Ptr); break; }
            case VT_U64   : { m_TypeSize = sizeof(u64   ); Set(*(u64   *)Ptr); break; }
            case VT_F32   : { m_TypeSize = sizeof(f32   ); Set(*(f32   *)Ptr); break; }
            case VT_F64   : { m_TypeSize = sizeof(f64   ); Set(*(f64   *)Ptr); break; }
            case VT_VEC2  : { m_TypeSize = sizeof(Vec2  ); Set(*(Vec2  *)Ptr); break; }
            case VT_VEC3  : { m_TypeSize = sizeof(Vec3  ); Set(*(Vec3  *)Ptr); break; }
            case VT_VEC4  : { m_TypeSize = sizeof(Vec4  ); Set(*(Vec4  *)Ptr); break; }
            case VT_MAT4  : { m_TypeSize = sizeof(Mat4  ); Set(*(Mat4  *)Ptr); break; }
            default:
            {
                ERROR("Invalid value type.\n");
                m_TypeSize = 0;
            }
        }
    }
    ConfigValue::~ConfigValue()
    {
        switch(m_Type)
        {
            case VT_BYTE  : { delete ( Byte *)m_TargetValue; break; }
            case VT_UBYTE : { delete (uByte *)m_TargetValue; break; }
            case VT_STRING: { delete (string*)m_TargetValue; break; }
            case VT_BOOL  : { delete (bool  *)m_TargetValue; break; }
            case VT_I16   : { delete (i16   *)m_TargetValue; break; }
            case VT_U16   : { delete (u16   *)m_TargetValue; break; }
            case VT_I32   : { delete (i32   *)m_TargetValue; break; }
            case VT_U32   : { delete (u32   *)m_TargetValue; break; }
            case VT_I64   : { delete (i64   *)m_TargetValue; break; }
            case VT_U64   : { delete (u64   *)m_TargetValue; break; }
            case VT_F32   : { delete (f32   *)m_TargetValue; break; }
            case VT_F64   : { delete (f64   *)m_TargetValue; break; }
            case VT_VEC2  : { delete (Vec2  *)m_TargetValue; break; }
            case VT_VEC3  : { delete (Vec3  *)m_TargetValue; break; }
            case VT_VEC4  : { delete (Vec4  *)m_TargetValue; break; }
            case VT_MAT4  : { delete (Mat4  *)m_TargetValue; break; }
            default:
            {
                ERROR("Invalid value type.\n");
                m_TypeSize = 0;
            }
        }
    }
    
    #define cvSetter(Type,Enum,FMT0,...) \
    void ConfigValue::Set(Type v) \
    { \
        if(m_Type != Enum) \
        { \
            ERROR("Could not set config value to " FMT0 ", invalid type. (Expected %s)\n",__VA_ARGS__,vtStrings[m_Type]); \
            return; \
        } \
        m_TargetValue = new Type(v); \
        m_DidChange = true; \
    }
    
    #define cvSetter_c(Type,Enum,FMT0,...) \
    void ConfigValue::Set(const Type& v) \
    { \
        if(m_Type != Enum) \
        { \
            ERROR("Could not set config value to " FMT0 ", invalid type. (Expected %s)\n",__VA_ARGS__,vtStrings[m_Type]); \
            return; \
        } \
        m_TargetValue = new Type(v); \
        m_DidChange = true; \
    }
    
    #define cvGetter(Type,Enum) \
    ConfigValue::operator Type() const \
    { \
        if(m_Type != Enum) \
        { \
            WARNING("Attempting to get config value with incorrect type (%s != %s). Expect unexpected results.\n",vtStrings[Enum],vtStrings[m_Type]); \
        } \
        return *(Type*)m_TargetValue; \
    } 
    
    cvSetter  (Byte  ,VT_BYTE  ,"Byte (0x%2X)"      ,v);
    cvSetter  (uByte ,VT_UBYTE ,"uByte (0x%2X)"     ,v);
    cvSetter_c(string,VT_STRING,"string (\"%s\")"   ,v.c_str());
    cvSetter  (bool  ,VT_BOOL  ,"bool (%s)"         ,(v ? "true" : "false"));
    cvSetter  (i16   ,VT_I16   ,"i16 (%d)"          ,v);
    cvSetter  (u16   ,VT_U16   ,"u16 (%d)"          ,v);
    cvSetter  (i32   ,VT_I32   ,"i32 (%d)"          ,v);
    cvSetter  (u32   ,VT_U32   ,"u32 (%d)"          ,v);
    cvSetter  (i64   ,VT_I64   ,"i64 (%lld)"        ,v);
    cvSetter  (u64   ,VT_U64   ,"u64 (%lld)"        ,v);
    cvSetter  (f32   ,VT_F32   ,"f32 (%f)"          ,v);
    cvSetter  (f64   ,VT_F64   ,"f64 (%f)"          ,v);
    cvSetter_c(Vec2  ,VT_VEC2  ,"Vec2 (%f,%f)"      ,v.x,v.y);
    cvSetter_c(Vec3  ,VT_VEC3  ,"Vec3 (%f,%f,%f)"   ,v.x,v.y,v.z);
    cvSetter_c(Vec4  ,VT_VEC4  ,"Vec4 (%f,%f,%f,%f)",v.x,v.y,v.z,v.w);
    cvSetter_c(Mat4  ,VT_MAT4  ,"Mat4 (%0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f)"
                                    ,v.x.x,v.x.y,v.x.z,v.x.w
                                    ,v.y.x,v.y.y,v.y.z,v.y.w
                                    ,v.z.x,v.z.y,v.z.z,v.z.w
                                    ,v.w.x,v.w.y,v.w.z,v.w.w);
    
    cvGetter(Byte  ,VT_BYTE  );
    cvGetter(uByte ,VT_UBYTE );
    cvGetter(string,VT_STRING);
    cvGetter(bool  ,VT_BOOL  );
    cvGetter(i16   ,VT_I16   );
    cvGetter(u16   ,VT_U16   );
    cvGetter(i32   ,VT_I32   );
    cvGetter(u32   ,VT_U32   );
    cvGetter(i64   ,VT_I64   );
    cvGetter(u64   ,VT_U64   );
    cvGetter(f32   ,VT_F32   );
    cvGetter(f64   ,VT_F64   );
    cvGetter(Vec2  ,VT_VEC2  );
    cvGetter(Vec3  ,VT_VEC3  );
    cvGetter(Vec4  ,VT_VEC4  );
    cvGetter(Mat4  ,VT_MAT4  );
    
    
    string ConfigValue::GetTypeName() const
    {
        return vtStrings[m_Type];
    }
    void ConfigValue::DebugPrintValue()
    {
        printf("%s",GetDebugPrintString().c_str());
    }
    string ConfigValue::GetDebugPrintString()
    {
        string Val;
        
        switch(m_Type)
        {
            case VT_BYTE  : { Val = FormatString("%s: %s(0x%2X)" ,vtStrings[m_Type], *(Byte  *)m_TargetValue); break; }
            case VT_UBYTE : { Val = FormatString("%s: %s(0x%2X)" ,vtStrings[m_Type], *(uByte *)m_TargetValue); break; }
            case VT_STRING: { Val = FormatString("%s: %s(\"%s\")",vtStrings[m_Type],(*(string*)m_TargetValue).c_str()); break; }
            case VT_BOOL  : { Val = FormatString("%s: %s(%s)"    ,vtStrings[m_Type],(*(bool  *)m_TargetValue) ? "true" : "false"); break; }
            case VT_I16   : { Val = FormatString("%s: %s(0x%2X)" ,vtStrings[m_Type], *(uByte *)m_TargetValue); break; }
            case VT_U16   : { Val = FormatString("%s: %s(%d)"    ,vtStrings[m_Type], *(u16   *)m_TargetValue); break; }
            case VT_I32   : { Val = FormatString("%s: %s(%d)"    ,vtStrings[m_Type], *(i32   *)m_TargetValue); break; }
            case VT_U32   : { Val = FormatString("%s: %s(%d)"    ,vtStrings[m_Type], *(u32   *)m_TargetValue); break; }
            case VT_I64   : { Val = FormatString("%s: %s(%d)"    ,vtStrings[m_Type], *(i64   *)m_TargetValue); break; }
            case VT_U64   : { Val = FormatString("%s: %s(%d)"    ,vtStrings[m_Type], *(u64   *)m_TargetValue); break; }
            case VT_F32   : { Val = FormatString("%s: %s(%f)"    ,vtStrings[m_Type], *(f32   *)m_TargetValue); break; }
            case VT_F64   : { Val = FormatString("%s: %s(%f)"    ,vtStrings[m_Type], *(f64   *)m_TargetValue); break; }
            
            case VT_VEC2  : { Vec2* v = (Vec2*)m_TargetValue; Val = FormatString("%s: %s(%0.4f,%0.4f)"            ,vtStrings[m_Type],v->x,v->y); break; }
            case VT_VEC3  : { Vec3* v = (Vec3*)m_TargetValue; Val = FormatString("%s: %s(%0.4f,%0.4f,%0.4f)"      ,vtStrings[m_Type],v->x,v->y,v->z); break; }
            case VT_VEC4  : { Vec4* v = (Vec4*)m_TargetValue; Val = FormatString("%s: %s(%0.4f,%0.4f,%0.4f,%0.4f)",vtStrings[m_Type],v->x,v->y,v->z,v->w); break; }
            case VT_MAT4  : { Mat4* v = (Mat4*)m_TargetValue; Val = FormatString("%s: %s(%0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f | %0.4f,%0.4f,%0.4f,%0.4f)",vtStrings[m_Type],v->x.x,v->x.y,v->x.z,v->x.w,
                                                                                                                                                                                                                   v->y.x,v->y.y,v->y.z,v->y.w,
                                                                                                                                                                                                                   v->z.x,v->z.y,v->z.z,v->z.w,
                                                                                                                                                                                                                   v->w.x,v->w.y,v->w.z,v->w.w);
                                break;
                            }
            default:
            {
                Val = "(invalid type)";
            }
            
        }
        
        return Val;
    }
    size_t ConfigValue::Serialize(Byte** OutDataPtr)
    {
        size_t TotalSize = 0;
        TotalSize += sizeof(i32   ); //Type
        TotalSize += sizeof(size_t); //Type size
        TotalSize += m_TypeSize;
        
        Byte* Buffer = (*OutDataPtr) = new Byte[TotalSize];
        memcpy(Buffer,&m_Type    ,sizeof(i32)); Buffer += sizeof(i32);
        memcpy(Buffer,&m_TypeSize,sizeof(i32)); Buffer += sizeof(i32);
        
        if(m_Type == VT_STRING) memcpy(Buffer,&(*((string*)m_TargetValue))[0],m_TypeSize);
        else memcpy(Buffer,m_TargetValue,m_TypeSize);
        
        return TotalSize;
    }
    size_t ConfigValue::Deserialize(Byte* InData)
    {
        i32 Type        = *(i32   *)InData; InData += sizeof(i32   );
        size_t TypeSize = *(size_t*)InData; InData += sizeof(size_t);
        
        if(Type != m_Type)
        {
            if(Type < VT_COUNT && Type >= 0) ERROR("Could not load config value from data, type mismatch. (%s != %s)\n",vtStrings[Type],vtStrings[m_Type]);
            else ERROR("Could not load config value from data, invalid type. (%d)\n",Type);
            return 0;
        }
        if(TypeSize != m_TypeSize)
        {
            ERROR("Could not load config value from data, size mismatch. (%zu != %zu)\n",TypeSize,m_TypeSize);
        }
        
        memcpy(m_TargetValue,InData,TypeSize);
        
        return sizeof(i32) + sizeof(size_t) + TypeSize;
    }
};
