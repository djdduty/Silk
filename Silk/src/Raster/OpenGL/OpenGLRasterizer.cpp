#include <Raster/OpenGL/OpenGLRasterizer.h>
#include <Raster/OpenGL/OpenGLUniform.h>
#include <Renderer/Mesh.h>
#include <Raster/OpenGL/OpenGLShader.h>
#include <Raster/OpenGL/OpenGLTexture.h>

namespace Silk
{
    OpenGLObject::OpenGLObject(Rasterizer* r) : RasterObject(r), m_VAO(0), m_IBIndex(-1), m_IsInstanced(false),
                m_MiniTransformSet(0), m_MaxiTransformSet(0), m_MiniNTransformSet(0), m_MaxiNTransformSet(0),
                m_MiniTTransformSet(0), m_MaxiTTransformSet(0), m_InstanceTransformsID(-1),
                m_InstanceNormalTransformsID(-1), m_InstanceTextureTransformsID(-1)
    {
    }
    OpenGLObject::~OpenGLObject()
    {
        ClearData();
    }
    void OpenGLObject::ClearData()
    {
        if(m_VAO) glDeleteVertexArrays(1,&m_VAO);
        m_VAO = 0;
        for(i32 i = 0;i < m_Attributes.size();i++)
        {
            glDeleteBuffers(1,&m_Attributes[i].BufferID);
        }
        m_Attributes.clear();
    }
    
    void OpenGLObject::SetMesh(Mesh* m)
    {
        ClearData();
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        i32 AttribCount = m->GetAttributeCount();
        for(i32 i = 0;i < AttribCount;i++)
        {
            const Mesh::MeshAttribute* Attrib = m->GetAttribute(i);
            if(Attrib->ShaderIndex == -1) //Index buffer
            {
                AddAttribute(-1,1,Attrib->Size,Attrib->Type,GL_FALSE,Attrib->Stride,0);
                SupplyBufferData(-1,GL_ELEMENT_ARRAY_BUFFER,1,Attrib->Size,Attrib->Pointer,Attrib->IsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
            
                SetIndexBufferAttributeIndex(-1);
            }
            else
            {
                AddAttribute(Attrib->ShaderIndex   ,
                             Attrib->ComponentCount,
                             Attrib->Size          ,
                             Attrib->Type          ,
                             GL_FALSE              ,
                             Attrib->Stride        ,
                             0);
                
                SupplyBufferData(Attrib->ShaderIndex,GL_ARRAY_BUFFER,Attrib->ComponentCount,Attrib->Size,Attrib->Pointer,Attrib->IsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
            }
        }
        glBindVertexArray(0);
    }
    void OpenGLObject::AddAttribute(GLuint AttribIndex,GLint ComponentCount,GLint Size,GLenum Type,GLboolean Normalized,GLsizei Stride,GLvoid* Pointer)
    {
        if(GetAttributeBufferIndex(AttribIndex) != -1)
        {
            ERROR("Attribute with index %d already exists in buffer. (Object: 0x%lX)\n",AttribIndex,(intptr_t)this);
            return;
        }
        AttributeBuffer b;
        glGenBuffers(1,&b.BufferID);
        
        if(b.BufferID == 0)
        {
            ERROR("Unable to create attribute buffer for attribute %d. (Object: 0x%lX)\n",AttribIndex,(intptr_t)this);
            return;
        }
        
        b.Index          = AttribIndex   ;
        b.Size           = Size          ;
        b.ComponentCount = ComponentCount;
        b.Type           = Type          ;
        b.Normalized     = Normalized    ;
        b.Stride         = Stride        ;
        b.Pointer        = Pointer       ;
        
        switch(Type)
        {
            case Mesh::AT_BYTE  : { b.TypeSize = sizeof(GLbyte  );b.Type = GL_BYTE          ; break; }
            case Mesh::AT_UBYTE : { b.TypeSize = sizeof(GLubyte );b.Type = GL_UNSIGNED_BYTE ; break; }
            case Mesh::AT_SHORT : { b.TypeSize = sizeof(GLshort );b.Type = GL_SHORT         ; break; }
            case Mesh::AT_USHORT: { b.TypeSize = sizeof(GLushort);b.Type = GL_UNSIGNED_SHORT; break; }
            case Mesh::AT_INT   : { b.TypeSize = sizeof(GLint   );b.Type = GL_INT           ; break; }
            case Mesh::AT_UINT  : { b.TypeSize = sizeof(GLuint  );b.Type = GL_UNSIGNED_INT  ; break; }
            case Mesh::AT_FLOAT : { b.TypeSize = sizeof(GLfloat );b.Type = GL_FLOAT         ; break; }
            case Mesh::AT_DOUBLE: { b.TypeSize = sizeof(GLdouble);b.Type = GL_DOUBLE        ; break; }
            default:
            {
                ERROR("Invalid attribute type 0x%X. (Object: 0x%lX)\n",Type,(intptr_t)this);
                glDeleteBuffers(1,&b.BufferID);
                return;
            }
        }
        
        m_Attributes.push_back(b);
    }
    void OpenGLObject::SupplyBufferData(GLuint AttributeIndex,GLenum Target,GLint ComponentCount,GLsizeiptr Size,GLvoid* Data,GLenum Usage)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        
        glBindBuffer(Target,m_Attributes[Index].BufferID);
        if(ComponentCount != 16)
        {
            if(AttributeIndex != -1)
            {
                glEnableVertexAttribArray(AttributeIndex);
                glVertexAttribPointer(AttributeIndex,ComponentCount,m_Attributes[Index].Type,m_Attributes[Index].Normalized,m_Attributes[Index].Stride,0);
            }
            glBufferData(Target,Size,Data,Usage);
        }
        else //For instanced rendering
        {
            for(i32 i = 0;i < 4;i++)
            {
                glEnableVertexAttribArray(AttributeIndex + i);
                glVertexAttribPointer(AttributeIndex + i,4,GL_FLOAT,GL_FALSE,sizeof(Mat4),(const GLvoid*)(sizeof(GLfloat) * i * 4));
                glVertexAttribDivisor(AttributeIndex + i,1);
            }
            glBufferData(Target,Size,Data,GL_DYNAMIC_DRAW);
        }
        glBindBuffer(Target,0);
    }
    bool OpenGLObject::IsInstanced()
    {
        return m_IsInstanced;
    }
    i32 OpenGLObject::AddInstance()
    {
        m_IsInstanced = true;
        m_InstanceCountChanged = true;
        m_InstanceTransforms       .push_back(Mat4::Identity);
        m_InstanceNormalTransforms .push_back(Mat4::Identity);
        if(m_Rasterizer->SupportsInstanceTextureTransforms()) m_InstanceTextureTransforms.push_back(Mat4::Identity);

        return m_InstanceTransforms.size() - 1;
    }
    i32 OpenGLObject::RemoveInstance(i32 InstanceID)
    {
        if(InstanceID >= m_InstanceTransforms.size()) return m_InstanceTransforms.size();
        m_InstanceCountChanged = true;
        m_InstanceTransforms       .erase(m_InstanceTransforms       .begin() + InstanceID);
        m_InstanceNormalTransforms .erase(m_InstanceNormalTransforms .begin() + InstanceID);
        if(m_Rasterizer->SupportsInstanceTextureTransforms())m_InstanceTextureTransforms.erase(m_InstanceTextureTransforms.begin() + InstanceID);
        
        if(m_InstanceTransforms.size() == 0)
        {
            m_IsInstanced = false;
            glDeleteBuffers(1,&m_Attributes[m_InstanceTransformsID       ].BufferID);
            m_Attributes.erase(m_Attributes.begin() + m_InstanceTransformsID);
            
            glDeleteBuffers(1,&m_Attributes[m_InstanceNormalTransformsID ].BufferID);
            m_Attributes.erase(m_Attributes.begin() + m_InstanceNormalTransformsID);
            
            if(m_InstanceTextureTransformsID != -1)
            {
                glDeleteBuffers(1,&m_Attributes[m_InstanceTextureTransformsID].BufferID);
                m_Attributes.erase(m_Attributes.begin() + m_InstanceTextureTransformsID);
                
            }
            m_InstanceTransformsID = m_InstanceNormalTransformsID = m_InstanceTextureTransformsID = -1;
        }
        
        return m_InstanceTransforms.size() - 1;
    }
    void OpenGLObject::SetInstanceTransform(i32 InstanceID,const Mat4& Transform)
    {
        if(InstanceID >= m_InstanceTransforms.size()) return;
        
        if(InstanceID < m_MiniTransformSet) m_MiniTransformSet = InstanceID;
        else if(InstanceID > m_MaxiTransformSet) m_MaxiTransformSet = InstanceID;
        
        m_InstanceTransforms[InstanceID] = Transform.Transpose();
    }
    void OpenGLObject::SetInstanceNormalTransform(i32 InstanceID,const Mat4& Transform)
    {
        if(InstanceID >= m_InstanceTransforms.size()) return;
        
        if(InstanceID < m_MiniNTransformSet) m_MiniNTransformSet = InstanceID;
        else if(InstanceID > m_MaxiNTransformSet) m_MaxiNTransformSet = InstanceID;
        
        m_InstanceNormalTransforms[InstanceID] = Transform.Transpose();
    }
    void OpenGLObject::SetInstanceTextureTransform(i32 InstanceID,const Mat4& Transform)
    {
        if(InstanceID >= m_InstanceTransforms.size() || !m_Rasterizer->SupportsInstanceTextureTransforms()) return;
        
        if(InstanceID < m_MiniTTransformSet) m_MiniTTransformSet = InstanceID;
        else if(InstanceID > m_MaxiTTransformSet) m_MaxiTTransformSet = InstanceID;
        
        m_InstanceTextureTransforms[InstanceID] = Transform.Transpose();
    }
    void OpenGLObject::UpdateInstanceData()
    {
        if(m_InstanceCountChanged)
        {
            m_MiniTransformSet = m_MiniNTransformSet = m_MiniTTransformSet =
            m_MaxiTransformSet = m_MaxiNTransformSet = m_MaxiTTransformSet = -1;
            
            glBindVertexArray(m_VAO);
            
            i32 Sz = sizeof(Mat4) * m_InstanceTransforms.size();
            if(m_InstanceTransformsID == -1)
            {
                m_InstanceTransformsID = m_Attributes.size();
                AddAttribute(InstanceTransformAttribIndex,16,Sz,Mesh::AT_FLOAT,GL_FALSE,0,0);
                
                m_InstanceNormalTransformsID = m_Attributes.size();
                AddAttribute(InstanceNormalTransformAttribIndex,16,Sz,Mesh::AT_FLOAT,GL_FALSE,0,0);
                
                if(m_Rasterizer->SupportsInstanceTextureTransforms())
                {
                    m_InstanceTextureTransformsID = m_Attributes.size();
                    AddAttribute(InstanceTextureTransformAttribIndex,16,Sz,Mesh::AT_FLOAT,GL_FALSE,0,0);
                }
            }
            
            m_Attributes[m_InstanceTransformsID      ].Size = sizeof(Mat4) * m_InstanceTransforms.size();
            m_Attributes[m_InstanceNormalTransformsID].Size = sizeof(Mat4) * m_InstanceTransforms.size();
            if(m_Rasterizer->SupportsInstanceTextureTransforms()) m_Attributes[m_InstanceTextureTransformsID].Size = sizeof(Mat4) * m_InstanceTransforms.size();
        
            SupplyBufferData(InstanceTransformAttribIndex       ,GL_ARRAY_BUFFER,16,Sz,&m_InstanceTransforms       [0].x.x,GL_DYNAMIC_DRAW);
            SupplyBufferData(InstanceNormalTransformAttribIndex ,GL_ARRAY_BUFFER,16,Sz,&m_InstanceNormalTransforms [0].x.x,GL_DYNAMIC_DRAW);
            if(m_Rasterizer->SupportsInstanceTextureTransforms()) SupplyBufferData(InstanceTextureTransformAttribIndex,GL_ARRAY_BUFFER,16,Sz,&m_InstanceTextureTransforms[0].x.x,GL_DYNAMIC_DRAW);
        
            glBindVertexArray(0);
        }
        else if(m_MiniTransformSet != -1)
        {
            glBindVertexArray(m_VAO);
            
            glBindBuffer   (GL_ARRAY_BUFFER,m_Attributes[m_InstanceTransformsID].BufferID);
            glBufferSubData(GL_ARRAY_BUFFER,m_MiniTransformSet  * sizeof(Mat4),(m_MaxiTransformSet  - m_MiniTransformSet ) * sizeof(Mat4),&m_InstanceTransforms      [m_MiniTransformSet].x.x);
            
            glBindBuffer   (GL_ARRAY_BUFFER,m_Attributes[m_InstanceNormalTransformsID].BufferID);
            glBufferSubData(GL_ARRAY_BUFFER,m_MiniNTransformSet * sizeof(Mat4),(m_MaxiNTransformSet - m_MiniNTransformSet) * sizeof(Mat4),&m_InstanceNormalTransforms[m_MiniTransformSet].x.x);
            
            if(m_Rasterizer->SupportsInstanceTextureTransforms())
            {
                glBindBuffer   (GL_ARRAY_BUFFER,m_Attributes[m_InstanceTextureTransformsID].BufferID);
                glBufferSubData(GL_ARRAY_BUFFER,m_MiniTTransformSet * sizeof(Mat4),(m_MaxiTTransformSet - m_MiniTTransformSet) * sizeof(Mat4),&m_InstanceTextureTransforms[m_MiniTransformSet].x.x);
            }
            
            m_MiniTransformSet = m_MiniNTransformSet = m_MiniTTransformSet =
            m_MaxiTransformSet = m_MaxiNTransformSet = m_MaxiTTransformSet = -1;
            
            glBindVertexArray(0);
        }
    }
    void OpenGLObject::SetIndexBufferAttributeIndex(GLuint AttributeIndex)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        m_IBIndex = Index;
    }
    void* OpenGLObject::MapBuffer(GLuint AttributeIndex,BUFFER_MAP_ACCESS Access)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return 0;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER,m_Attributes[Index].BufferID);
        switch(Access)
        {
            case BMA_READ:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_READ_ONLY);
                break;
            }
            case BMA_WRITE:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
                break;
            }
            case BMA_READWRITE:
            {
                return glMapBuffer(GL_ARRAY_BUFFER,GL_READ_WRITE);
                break;
            }
        }
        return 0;
    }
    void OpenGLObject::UnmapBuffer(GLuint AttributeIndex)
    {
        i32 Index = GetAttributeBufferIndex(AttributeIndex);
        if(Index == -1)
        {
            ERROR("Attribute with index %d does not exists in buffer. (Object: 0x%lX)\n",AttributeIndex,(intptr_t)this);
            return;
        }
        glBindBuffer(GL_ARRAY_BUFFER,m_Attributes[Index].BufferID);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    void OpenGLObject::Render(i32 PrimitiveType,i32 Start,i32 Count)
    {
        if(m_VAO == 0 || m_Attributes.size() == 0) return;

        glBindVertexArray(m_VAO);
        
        if(m_IsInstanced)
        {
            if(m_IBIndex != -1)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_Attributes[m_IBIndex].BufferID);
                glDrawElementsInstanced(PrimitiveType,
                                        Count,
                                        m_Attributes[m_IBIndex].Type,
                                        (GLvoid*)(Start * m_Attributes[m_IBIndex].TypeSize),
                                        m_InstanceTransforms.size());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
            }
            else glDrawArraysInstanced(PrimitiveType,Start,Count,m_InstanceTransforms.size());
        }
        else
        {
            if(m_IBIndex != -1)
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_Attributes[m_IBIndex].BufferID);
                glDrawElements(PrimitiveType,
                               Count,
                               m_Attributes[m_IBIndex].Type,
                               (GLvoid*)(Start * m_Attributes[m_IBIndex].TypeSize));
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
            }
            else glDrawArrays(PrimitiveType,Start,Count);
        }
        
        glBindVertexArray(0);
    }
    i32 OpenGLObject::GetAttributeBufferIndex(i32 AttribIndex) const
    {
        for(i32 i = 0;i < m_Attributes.size();i++) { if(m_Attributes[i].Index == AttribIndex) { return i; } }
        return -1;
    }
    
    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
        if(m_Buffer != 0) glDeleteBuffers(1,&m_Buffer);
        m_Buffer = 0;
    }
    void OpenGLUniformBuffer::InitializeBuffer()
    {
        return;
        if(m_Buffer) glDeleteBuffers(1,&m_Buffer);
        glGenBuffers(1,&m_Buffer);
        
        UpdateBuffer();
    }
    void OpenGLUniformBuffer::UpdateBuffer()
    {
        i32 TotalSize = GetPaddedBufferSize();
        
        Byte* UBO = new Byte[TotalSize];
        memset(UBO,0,TotalSize);
        for(i32 i = 0;i < m_UniformInfo.size();i++)
        {
            i32 Offset = GetPaddedUniformOffset(i);
            i32 Padding = m_UniformInfo[i]->PaddedSize - m_UniformInfo[i]->TypeSize;
            
            if(m_UniformInfo[i]->ArraySize != -1 && Padding != 0)
            {
                i32 dOffset = 0;
                for(i32 u = 0;u < m_UniformInfo[i]->ArraySize;u++)
                {
                    memcpy(&UBO[Offset + dOffset],&((Byte*)m_UniformBuffer[i])[u * m_UniformInfo[i]->TypeSize],m_UniformInfo[i]->TypeSize);
                    dOffset += m_UniformInfo[i]->PaddedSize;
                }
            }
            else
            {
                //printf("v[%d]: %s | o: %d | s: %d | p: %d\n",i,m_UniformInfo[i].Name.c_str(),Offset,m_UniformInfo[i].TypeSize,Padding);
                memcpy(&UBO[Offset],m_UniformBuffer[i],m_UniformInfo[i]->Size);
            }
        }
        
        /*
        f32* f = (f32*)UBO;
        for(i32 i = 0;i < TotalSize / 4;i += 4)
        {
            printf("[%d]%f,%f,%f,%f\n",4 * i,f[i],f[i + 1],f[i + 2],f[i + 3]);
        }
        */
        
        glBindBuffer(GL_UNIFORM_BUFFER,m_Buffer);
        glBufferData(GL_UNIFORM_BUFFER,TotalSize,UBO,GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER,0);
        
        delete UBO;
    }
    
    OpenGLRasterizerContext::~OpenGLRasterizerContext()
    {
    }

    OpenGLRasterizer::OpenGLRasterizer()
    {
    }
    OpenGLRasterizer::~OpenGLRasterizer()
    {
    }
    
    #define FormatCheck4(r,g,b,a) (f.rbits == (r) && f.gbits == (g) && f.bbits == (b) && f.abits == (a))
    #define FormatCheck3(r,g,b)   FormatCheck4(r,g,b,f.abits)
    #define FormatCheckG(g,a)     (f.graybits == (g) && f.abits == (a))
    
    bool OpenGLRasterizer::ValidateContext(RasterContext *Ctx)
    {
        ColorFormat &f = Ctx->m_Format;
        switch(f.Channels)
        {
            case ColorFormat::CC_RGB:
            {
                if     (FormatCheck3(3 ,3 ,2    )) m_ColorFormat = GL_R3_G3_B2              ;
                else if(FormatCheck3(4 ,4 ,4    )) m_ColorFormat = GL_RGB4                  ;
                else if(FormatCheck3(5 ,5 ,5    )) m_ColorFormat = GL_RGB5                  ;
                else if(FormatCheck3(8 ,8 ,8    )) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck3(10,10,10   )) m_ColorFormat = GL_RGB10                 ;
                else if(FormatCheck3(12,12,12   )) m_ColorFormat = GL_RGB12                 ;
                else if(FormatCheck3(16,16,16   )) m_ColorFormat = GL_RGB16                 ;
                else
                {
                    ERROR("Incompatible color format CC_RGB (%d,%d,%d)\n",(i32)f.rbits,
                                                                          (i32)f.gbits,
                                                                          (i32)f.bbits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_RGBA:
            {
                if     (FormatCheck4(2 , 2, 2, 2)) m_ColorFormat = GL_RGBA2                 ;
                else if(FormatCheck4(4 , 4, 4, 4)) m_ColorFormat = GL_RGBA4                 ;
                else if(FormatCheck4(5 , 5, 5, 1)) m_ColorFormat = GL_RGB5_A1               ;
                else if(FormatCheck4(8 , 8, 8, 8)) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck4(10,10,10, 2)) m_ColorFormat = GL_RGB10_A2              ;
                else if(FormatCheck4(12,12,12,12)) m_ColorFormat = GL_RGBA12                ;
                else if(FormatCheck4(16,16,16,16)) m_ColorFormat = GL_RGBA16                ;
                else
                {
                    ERROR("Incompatible color format CC_RGBA (%d,%d,%d,%d)\n",(i32)f.rbits,
                                                                              (i32)f.gbits,
                                                                              (i32)f.bbits,
                                                                              (i32)f.abits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_GRAYSCALE:
            {
                if     (FormatCheckG(4 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(8 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(12,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(16,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(4 ,4       )) m_ColorFormat = GL_LUMINANCE4_ALPHA4     ;
                else if(FormatCheckG(6 ,2       )) m_ColorFormat = GL_LUMINANCE6_ALPHA2     ;
                else if(FormatCheckG(8 ,8       )) m_ColorFormat = GL_LUMINANCE8_ALPHA8     ;
                else if(FormatCheckG(12,4       )) m_ColorFormat = GL_LUMINANCE12_ALPHA4    ;
                else if(FormatCheckG(12,12      )) m_ColorFormat = GL_LUMINANCE12_ALPHA12   ;
                else if(FormatCheckG(16,16      )) m_ColorFormat = GL_LUMINANCE16_ALPHA16   ;
                else
                {
                    if(f.abits == 0) ERROR("Incompatible color format CC_GRAYSCALE%d\n"        ,(i32)f.graybits);
                    else             ERROR("Incompatible color format CC_GRAYSCALE (%d,A:%d)\n",(i32)f.graybits,
                                                                                                (i32)f.abits);
                    return false;
                }
                break;
            }
            default:
            {
                ERROR("Incompatible color format: Invalid value for ColorFormat::Channels. (Memory corruption?)\n");
                return false;
            }
        }
        
        return true;
    }
    void OpenGLRasterizer::InitializeContext()
    {  
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if(err != GLEW_OK)
        {
            ERROR("Glew initialization failed: %s\n", glewGetErrorString(err));
        }
        glGetError();
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    bool OpenGLRasterizer::SupportsInstanceTextureTransforms()
    {
        i32 r = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&r);
        if(r < 18) return false;
        return true;
    }

    UniformBuffer* OpenGLRasterizer::CreateUniformBuffer(ShaderGenerator::INPUT_UNIFORM_TYPE Type)
    {
        UniformBuffer* ub = new OpenGLUniformBuffer();
        ub->SetUniformBlockInfo(GetUniformBlockTypeName(Type),Type);
        ub->InitializeBuffer();
        return ub;
    }
    Shader* OpenGLRasterizer::CreateShader()
    {
        //For now
        return new OpenGLShader(m_Renderer);
    }
    Texture* OpenGLRasterizer::CreateTexture()
    {
        return new OpenGLTexture();
    }

    void OpenGLRasterizer::Destroy(UniformBuffer* Buffer)
    {
        Buffer->ClearData();
        delete (OpenGLUniformBuffer*)Buffer;
    }
    void OpenGLRasterizer::Destroy(Shader *S)
    {
        delete (OpenGLShader*)S;
    }
    void OpenGLRasterizer::Destroy(Texture* T)
    {
        T->FreeMemory();
        delete (OpenGLTexture*)T;
    }
    void OpenGLRasterizer::Destroy(RasterObject* O)
    {
        delete (OpenGLObject*)O;
    }
};
