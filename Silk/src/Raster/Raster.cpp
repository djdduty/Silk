#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLShader.h>
#include <Raster/OpenGL/OpenGLRasterizer.h>

namespace Silk
{
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
    
    void UniformBuffer::ClearData()
    {
        for(i32 i = 0;i < m_UniformInfo.size();i++)
        {
            if(m_UniformBuffer[i])
            {
                switch(m_UniformInfo[i].Type)
                {
                    #define TypeCase(T,U) \
                    case U: \
                    { \
                        delete (T*)m_UniformBuffer[i]; \
                        break; \
                    }
                    TypeCase(bool,UT_BOOL  )
                    TypeCase(i32 ,UT_INT   )
                    TypeCase(u32 ,UT_UINT  )
                    TypeCase(f32 ,UT_FLOAT )
                    TypeCase(f64 ,UT_DOUBLE)
                    TypeCase(Vec2,UT_VEC2  )
                    TypeCase(Vec3,UT_VEC3  )
                    TypeCase(Vec4,UT_VEC4  )
                    TypeCase(Mat4,UT_MAT4  )
                    default:
                    {
                        ERROR("Invalid uniform type. Cannot delete. Memory corruption?\n");
                    }
                }
            }
        }
        m_UniformBuffer.clear();
        m_UniformInfo.clear();
    }
    
    i32 UniformBuffer::DefineUniform(string Name)
    {
        for(i32 i = 0;i < m_UniformInfo.size();i++)
        {
            if(m_UniformInfo[i].Name == Name)
            {
                ERROR("Error defining uniform. Uniform \"%s\" already exists. (Buffer: 0x%lX)\n",Name.c_str(),(intptr_t)this);
                return i;
            }
        }
        
        UniformDef u;
        u.Name = Name;
        m_UniformInfo.push_back(u);
        m_UniformBuffer.push_back(0);
        return m_UniformInfo.size() - 1;
    }
    #define SetUniformFunc(T,Enum,Sz) \
    void UniformBuffer::SetUniform(i32 UID,const T& Value) \
    { \
        if(m_UniformBuffer[UID] == 0) \
        { \
            m_UniformBuffer[UID] = new T(Value); \
            m_UniformInfo[UID].Type = Enum; \
            m_UniformInfo[UID].Size = Sz; \
            m_UniformInfo[UID].Offset = GetUniformOffset(UID); \
        } \
        if(m_UniformInfo[UID].Type != Enum) \
        { \
            ERROR("Invalid type (%s) for uniform. Uniform type is (%s).\n",#T,UniformTypeNames[m_UniformInfo[UID].Type]); \
            return; \
        } \
        (*(T*)m_UniformBuffer[UID]) = Value; \
    }
    
    SetUniformFunc(bool,UT_BOOL  ,sizeof(bool));
    SetUniformFunc(i32 ,UT_INT   ,sizeof(i32 ));
    SetUniformFunc(u32 ,UT_UINT  ,sizeof(u32 ));
    SetUniformFunc(f32 ,UT_FLOAT ,sizeof(f32 ));
    SetUniformFunc(f64 ,UT_DOUBLE,sizeof(f64 ));
    SetUniformFunc(Vec2,UT_VEC2  ,sizeof(Vec2));
    SetUniformFunc(Vec3,UT_VEC3  ,sizeof(Vec3));
    SetUniformFunc(Vec4,UT_VEC4  ,sizeof(Vec4));
    SetUniformFunc(Mat4,UT_MAT4  ,sizeof(Mat4));
    
    i32 UniformBuffer::GetUniformOffset(i32 UID) const
    {
        i32 Offset = 0;
        for(i32 i = 0;i < UID;i++) Offset += m_UniformInfo[i].Size;
        return Offset;
    }
    
    void Shader::AddUniformBuffer(UniformBuffer *Uniforms)
    {
        if(Uniforms->m_Parent)
        {
            if(Uniforms->m_Parent == this) return;
            else Uniforms->m_Parent->RemoveUniformBuffer(Uniforms);
        }
        m_UniformBuffers.push_back(Uniforms);
        Uniforms->m_Parent      = this;
        Uniforms->m_ParentIndex = m_UniformBuffers.size() - 1;
    }
    void Shader::RemoveUniformBuffer(UniformBuffer *Uniforms)
    {
        m_UniformBuffers.erase(m_UniformBuffers.begin() + Uniforms->m_ParentIndex);
        Uniforms->m_Parent = 0;
        for(i32 i = Uniforms->m_ParentIndex;i < m_UniformBuffers.size();i++)
        {
            m_UniformBuffers[i]->m_ParentIndex--;
        }
        Uniforms->m_ParentIndex = 0;
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
    UniformBuffer* Rasterizer::CreateUniformBuffer(ShaderGenerator::INPUT_UNIFORM_TYPE Type)
    {
        UniformBuffer* ub = new OpenGLUniformBuffer();
        ub->SetUniformBlockInfo(GetUniformBlockTypeName(Type),Type);
        return ub;
    }
    void Rasterizer::DestroyUniformBuffer(UniformBuffer* Buffer)
    {
        Buffer->ClearData();
        delete (OpenGLUniformBuffer*)Buffer;
    }
    Shader* Rasterizer::CreateShader()
    {
        //For now
        return new OpenGLShader();
    }
    void Rasterizer::DestroyShader(Shader *S)
    {
        delete (OpenGLShader*)S;
    }
};
