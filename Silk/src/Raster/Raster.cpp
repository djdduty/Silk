#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>
#include <Raster/OpenGL/OpenGLTexture.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>
#include <Raster/OpenGL/OpenGLUniform.h>

namespace Silk
{
    i32 RasterObject::Destroy(RenderObject *Caller)
    {
        Caller->m_Object = 0;
        Caller->m_Type   = ROT_COUNT;
        m_RefCount--;
        if(m_RefCount == 0) { m_Rasterizer->Destroy(this); return 0; }
        return m_RefCount;
    }
    
    static const char* UniformTypeNames[] =
    {
        "bool",
        "int",
        "uint",
        "float",
        "double",
        "Vec2",
        "Vec3",
        "Vec4",
        "Mat4",
    };
    
    UniformDef::~UniformDef()
    {
        for(i32 i = 0;i < PassCalls.size();i++)
        {
            delete PassCalls[i];
        }
        PassCalls.clear();
    }
    
    void UniformBuffer::ClearData()
    {
        for(i32 i = 0;i < m_UniformBuffer.size();i++)
        {
            if(m_UniformBuffer[i])
            {
                switch(m_UniformInfo[i]->Type)
                {
                    #define TypeCase(T,U) \
                    case U: \
                    { \
                        if(m_UniformInfo[i]->ArraySize != -1) delete [] (T*)m_UniformBuffer[i]; \
                        else if(m_UniformBuffer[i]) delete (T*)m_UniformBuffer[i]; \
                        break; \
                    }
                    TypeCase(bool ,UT_BOOL  )
                    TypeCase(i32  ,UT_INT   )
                    TypeCase(u32  ,UT_UINT  )
                    TypeCase(f32  ,UT_FLOAT )
                    TypeCase(f64  ,UT_DOUBLE)
                    TypeCase(Vec2 ,UT_VEC2  )
                    TypeCase(Vec3 ,UT_VEC3  )
                    TypeCase(Vec4 ,UT_VEC4  )
                    TypeCase(Mat4 ,UT_MAT4  )
                    case UT_LIGHT:
                    {
                        if(m_UniformInfo[i]->ArraySize != -1) delete [] (Light**)m_UniformBuffer[i];
                        else if(m_UniformBuffer[i]) delete (Light*)m_UniformBuffer[i];
                        break;
                    }
                    default:
                    {
                        ERROR("Invalid uniform type. Cannot delete. Memory corruption?\n");
                    }
                }
                delete m_UniformInfo[i];
            }
        }
        m_UniformBuffer.clear();
        m_UniformInfo.clear();
        m_TotalSize = m_TotalPaddedSize = 0;
    }
    
    i32 UniformBuffer::DefineUniform(string Name)
    {
        for(i32 i = 0;i < m_UniformInfo.size();i++)
        {
            if(m_UniformInfo[i]->Name == Name)
            {
                ERROR("Error defining uniform. Uniform \"%s\" already exists. (Buffer: 0x%lX)\n",Name.c_str(),(intptr_t)this);
                return i;
            }
        }
        
        UniformDef* u = new UniformDef();
        u->Name = Name;
        m_UniformInfo.push_back(u);
        m_UniformBuffer.push_back(0);
        return m_UniformInfo.size() - 1;
    }
    #define SetUniformFunc(C,T,Enum,Sz,PSz)                                     \
    void UniformBuffer::SetUniform(i32 UID,const T& Value)                      \
    {                                                                           \
        if(m_UniformBuffer[UID] == 0)                                           \
        {                                                                       \
            m_UniformBuffer[UID] = new T(Value);                                \
            m_UniformInfo[UID]->PassCalls.push_back(new C(m_UniformInfo[UID])); \
            m_UniformInfo[UID]->Type = Enum;                                    \
            m_UniformInfo[UID]->TypeSize = Sz;                                  \
            m_UniformInfo[UID]->Size = Sz;                                      \
            m_UniformInfo[UID]->PaddedSize = PSz;                               \
            m_UniformInfo[UID]->Offset = GetUniformOffset(UID);                 \
            m_TotalSize += m_UniformInfo[UID]->Size;                            \
            m_TotalPaddedSize += m_UniformInfo[UID]->PaddedSize;                \
            m_UniformInfo[UID]->ArraySize = -1;                                 \
        }                                                                       \
        if(m_UniformInfo[UID]->Type != Enum)                                    \
        {                                                                       \
            ERROR("Invalid type (%s) for uniform. Uniform type is (%s).\n",#T,UniformTypeNames[m_UniformInfo[UID]->Type]); \
            return;                                                             \
        }                                                                       \
        (*(T*)m_UniformBuffer[UID]) = Value;                                    \
       m_UpdatedUniforms.push_back(UID);                                        \
    }                                                                           \
    void UniformBuffer::SetUniform(i32 UID,const vector<T>& Values)             \
    {                                                                           \
        if(m_UniformBuffer[UID] == 0)                                           \
        {                                                                       \
            m_UniformBuffer[UID] = new T[Values.size()];                        \
            m_UniformInfo[UID]->PassCalls.push_back(new C(m_UniformInfo[UID])); \
            m_UniformInfo[UID]->Type = Enum;                                    \
            m_UniformInfo[UID]->TypeSize = Sz;                                  \
            m_UniformInfo[UID]->Size = Sz * Values.size();                      \
            m_UniformInfo[UID]->PaddedSize = PSz;                               \
            m_UniformInfo[UID]->Offset = GetUniformOffset(UID);                 \
            m_TotalSize += m_UniformInfo[UID]->Size;                            \
            m_TotalPaddedSize += PSz * Values.size();                           \
            m_UniformInfo[UID]->ArraySize = Values.size();                      \
        }                                                                       \
        if(m_UniformInfo[UID]->Type != Enum)                                    \
        {                                                                       \
            ERROR("Invalid type (%s) for uniform. Uniform type is (%s).\n",#T,UniformTypeNames[m_UniformInfo[UID]->Type]); \
            return;                                                             \
        }                                                                       \
        for(i32 i = 0;i < Values.size();i++)                                    \
        {                                                                       \
            ((T*)m_UniformBuffer[UID])[i] = Values[i];                          \
        }                                                                       \
       m_UpdatedUniforms.push_back(UID);                                        \
    }
    
    SetUniformFunc(UC_Bool  ,bool,UT_BOOL  ,sizeof(bool),sizeof(f32));// * 4 );
    SetUniformFunc(UC_Int   ,i32 ,UT_INT   ,sizeof(i32 ),sizeof(f32));// * 4 );
    SetUniformFunc(UC_UInt  ,u32 ,UT_UINT  ,sizeof(u32 ),sizeof(f32));// * 4 );
    SetUniformFunc(UC_Float ,f32 ,UT_FLOAT ,sizeof(f32 ),sizeof(f32));// * 4 );
    SetUniformFunc(UC_Double,f64 ,UT_DOUBLE,sizeof(f64 ),sizeof(f64));// * 4 );
    SetUniformFunc(UC_Vec2  ,Vec2,UT_VEC2  ,sizeof(Vec2),sizeof(f32) * 2 );
    SetUniformFunc(UC_Vec3  ,Vec3,UT_VEC3  ,sizeof(Vec3),sizeof(f32) * 4 );
    SetUniformFunc(UC_Vec4  ,Vec4,UT_VEC4  ,sizeof(Vec4),sizeof(f32) * 4 );
    SetUniformFunc(UC_Mat4  ,Mat4,UT_MAT4  ,sizeof(Mat4),sizeof(f32) * 16);
    
    void UniformBuffer::SetUniform(i32 UID,Light* Lt)
    {
        if(m_UniformBuffer[UID] == 0)
        {
            m_UniformBuffer[UID] = Lt;
            m_UniformInfo[UID]->PassCalls.push_back(new UC_Light(m_UniformInfo[UID]));
            m_UniformInfo[UID]->Type = UT_LIGHT;
            m_UniformInfo[UID]->Size = sizeof(Light);
            m_UniformInfo[UID]->PaddedSize = sizeof(Light);
            m_UniformInfo[UID]->TypeSize = sizeof(Light);
            m_UniformInfo[UID]->Offset = GetUniformOffset(UID);
            m_TotalSize += m_UniformInfo[UID]->Size;
            m_TotalPaddedSize += m_UniformInfo[UID]->PaddedSize;
            m_UniformInfo[UID]->ArraySize = -1;
        }
        if(m_UniformInfo[UID]->Type != UT_LIGHT)
        {
            ERROR("Invalid type (Light) for uniform. Uniform type is (%s).\n",UniformTypeNames[m_UniformInfo[UID]->Type]);
            return;
        }
        (*(Light*)m_UniformBuffer[UID]) = *Lt;
        m_UpdatedUniforms.push_back(UID);
    }
    void UniformBuffer::SetUniform(i32 UID,const vector<Light*>& Lts)
    {
        if(m_UniformBuffer[UID] == 0)
        {
            m_UniformBuffer[UID] = new Light*[Lts.size()];
            m_UniformInfo[UID]->PassCalls.push_back(new UC_Light(m_UniformInfo[UID]));
            m_UniformInfo[UID]->Type = UT_LIGHT;
            m_UniformInfo[UID]->Size = sizeof(Light) * Lts.size();
            m_UniformInfo[UID]->PaddedSize = sizeof(Light) * Lts.size();
            m_UniformInfo[UID]->TypeSize = sizeof(Light);
            m_UniformInfo[UID]->Offset = GetUniformOffset(UID);
            m_TotalSize += m_UniformInfo[UID]->Size;
            m_TotalPaddedSize += m_UniformInfo[UID]->PaddedSize * Lts.size();
            m_UniformInfo[UID]->ArraySize = Lts.size();
        }
        if(m_UniformInfo[UID]->Type != UT_LIGHT)
        {
            ERROR("Invalid type (Light) for uniform. Uniform type is (%s).\n",UniformTypeNames[m_UniformInfo[UID]->Type]);
            return;
        }
        for(i32 i = 0;i < Lts.size();i++)
        {
            ((Light**)m_UniformBuffer[UID])[i] = Lts[i];
        }
       m_UpdatedUniforms.push_back(UID);
    }

    
    i32 UniformBuffer::GetUniformOffset(i32 UID) const
    {
        i32 Offset = 0;
        for(i32 i = 0;i < UID;i++) Offset += m_UniformInfo[i]->Size;
        return Offset;
    }
    i32 UniformBuffer::GetPaddedUniformOffset(i32 UID) const
    {
        i32 Offset = 0;
        for(i32 i = 0;i < UID;i++)
        {
            if(m_UniformInfo[i]->ArraySize != -1) Offset += m_UniformInfo[i]->PaddedSize * m_UniformInfo[i]->ArraySize;
            else Offset += m_UniformInfo[i]->PaddedSize;
        }
        return Offset;
    }
    Shader::Shader(Renderer* r) : m_Renderer(r)
    {
        for(i32 i = 0;i < ShaderGenerator::IUT_COUNT;i++) m_UniformInputs  [i] = false;
        for(i32 i = 0;i < ShaderGenerator::OFT_COUNT;i++) m_FragmentOutputs[i] = false;
        m_SupportsInstancing = false;
    }
    
    void RasterContext::SetResolution(const Vec2& Resolution) 
    {
        m_Resolution = Resolution;
        if(m_Parent)
            m_Parent->SetViewport(0,0,m_Resolution.x, m_Resolution.y);
    }

    Rasterizer::Rasterizer()
    {
    }
    Rasterizer::~Rasterizer()
    {
        if(m_GraphicsContext)
            delete m_GraphicsContext;
    }
    
    bool Rasterizer::ValidateContext(RasterContext *Ctx)
    {
        ERROR("Cannot validate graphics context: Null rasterizer.\n");
        return false;
    }
    bool Rasterizer::SetContext(RasterContext *Ctx)
    {
        if(ValidateContext(Ctx))
        {
            m_GraphicsContext = Ctx;
            LOG("Valid graphics context.\n");
            return true;
        }
        return false;
    }
};
