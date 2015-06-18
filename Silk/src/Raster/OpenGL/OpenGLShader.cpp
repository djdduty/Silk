#include <Raster/OpenGL/OpenGLShader.h>
#include <Renderer/Mesh.h>

namespace Silk
{
    OpenGLShader::OpenGLShader()
    {
    }
    OpenGLShader::~OpenGLShader()
    {
    }

    i32 OpenGLShader::Load(CString VertexCode,CString GeometryCode,CString FragmentCode)
    {
        /*
        GLuint ugh;
        glGenVertexArrays(1,&ugh);
        glBindVertexArray(ugh);
        */
        
        m_VS = glCreateShader(GL_VERTEX_SHADER);
        m_PS = glCreateShader(GL_FRAGMENT_SHADER);
        
        /* Compile sources */
        i32 VtxLen = strlen(VertexCode);
        glShaderSource(m_VS,1,&VertexCode,&VtxLen);
        glCompileShader(m_VS);
        GLint Status;
        
        glGetShaderiv(m_VS,GL_COMPILE_STATUS,&Status);
        if(!Status)
        {
            GLint LogLen;
            glGetShaderiv(m_VS,GL_INFO_LOG_LENGTH,&LogLen);
            char iLog[LogLen];
            glGetShaderInfoLog(m_VS,LogLen,&LogLen,iLog);
        
            ERROR("Unable to compile vertex shader. (Shader: 0x%lX)\n",(intptr_t)this);
            ERROR("Info: %s\n",iLog);
            glDeleteShader(m_VS);
            glDeleteShader(m_PS);
            m_VS = m_PS = 0;
            //glBindVertexArray(0);
            //glDeleteVertexArrays(1,&ugh);
            return false;
        }
        
        if(GeometryCode)
        {
            m_GS = glCreateShader(GL_GEOMETRY_SHADER);
            i32 GeoLen = strlen(GeometryCode);
            glShaderSource(m_GS,1,&GeometryCode,&GeoLen);
            glCompileShader(m_GS);
            GLint Status;
            
            glGetShaderiv(m_GS,GL_COMPILE_STATUS,&Status);
            if(!Status)
            {
                GLint LogLen;
                glGetShaderiv(m_GS,GL_INFO_LOG_LENGTH,&LogLen);
                char iLog[LogLen];
                glGetShaderInfoLog(m_GS,LogLen,&LogLen,iLog);
            
                ERROR("Unable to compile geometry shader. (Shader: 0x%lX)\n",(intptr_t)this);
                ERROR("Info: %s\n",iLog);
                glDeleteShader(m_VS);
                glDeleteShader(m_GS);
                glDeleteShader(m_PS);
                m_VS = m_GS = m_PS = 0;
                //glBindVertexArray(0);
                //glDeleteVertexArrays(1,&ugh);
                return false;
            }
        }
        
        i32 FrgLen = strlen(FragmentCode);
        glShaderSource(m_PS,1,&FragmentCode,&FrgLen);
        glCompileShader(m_PS);
        
        glGetShaderiv(m_PS,GL_COMPILE_STATUS,&Status);
        if(!Status)
        {
            GLint LogLen;
            glGetShaderiv(m_PS,GL_INFO_LOG_LENGTH,&LogLen);
            char iLog[LogLen];
            glGetShaderInfoLog(m_PS,LogLen,&LogLen,iLog);
        
            ERROR("Unable to compile fragment shader. (Shader: 0x%lX)\n",(intptr_t)this);
            ERROR("Info: %s\n",iLog);
            glDeleteShader(m_VS);
            if(m_GS) glDeleteShader(m_GS);
            glDeleteShader(m_PS);
            m_VS = m_GS = m_PS = 0;
            //glBindVertexArray(0);
            //glDeleteVertexArrays(1,&ugh);
            return false;
        }
        
        /* Link program */
        m_PID = glCreateProgram();
        glAttachShader(m_PID,m_VS);
        if(m_GS) glAttachShader(m_PID,m_GS);
        glAttachShader(m_PID,m_PS);
        
        /*
        for(i32 i = 0;i < MAX_TEXTURES;i++)
        {
            char OutName[8];
            memset(OutName,0,8);
            snprintf(OutName,8,"Out%d",i);
            glBindFragDataLocation(m_PID,i,OutName);
        }
        */
        
        glBindAttribLocation(m_PID,PositionAttribIndex  ,PositionAttribName  );
        glBindAttribLocation(m_PID,NormalAttribIndex    ,NormalAttribName    );
        glBindAttribLocation(m_PID,ColorAttribIndex     ,ColorAttribName     );
        glBindAttribLocation(m_PID,TangentAttribIndex   ,TangentAttribName   );
        glBindAttribLocation(m_PID,TexCoordAttribIndex  ,TexCoordAttribName  );
        glBindAttribLocation(m_PID,BoneWeightAttribIndex,BoneWeightAttribName);
        glBindAttribLocation(m_PID,BoneIndexAttribIndex ,BoneIndexAttribName );
        
        glLinkProgram(m_PID);
        glGetProgramiv(m_PID,GL_LINK_STATUS,&Status);
        if(!Status)
        {
            GLint LogLen;
            glGetProgramiv(m_PS,GL_INFO_LOG_LENGTH,&LogLen);
            char iLog[LogLen];
            glGetProgramInfoLog(m_PID,LogLen,&LogLen,iLog);
            ERROR("Unable to link shader. (Shader: 0x%lX)\n",(intptr_t)this);
            ERROR("Info: %s\n",iLog);
            glDeleteProgram(m_PID);
            glDeleteShader(m_VS);
            glDeleteShader(m_PS);
            m_PID = m_VS = m_PS = 0;
            //glBindVertexArray(0);
            //glDeleteVertexArrays(1,&ugh);
            return false;
        }
        
        glValidateProgram(m_PID);
        glGetProgramiv(m_PID,GL_VALIDATE_STATUS,&Status);
        if(!Status)
        {
            GLint LogLen;
            glGetProgramiv(m_PID,GL_INFO_LOG_LENGTH,&LogLen);
            char iLog[LogLen];
            glGetProgramInfoLog(m_PID,LogLen,&LogLen,iLog);
            ERROR("Unable to validate shader. (Shader: 0x%lX)\n",(intptr_t)this);
            ERROR("Info: %s\n",iLog);
            glDeleteProgram(m_PID);
            glDeleteShader(m_VS);
            glDeleteShader(m_PS);
            m_PID = m_VS = m_PS = 0;
            //glBindVertexArray(0);
            //glDeleteVertexArrays(1,&ugh);
            return false;
        }
        
        //for(i32 i = 0;i < SU_COUNT;i++) m_UniformLocs[i] = glGetUniformLocation(m_PID,UniformNames[i]);
        
        //glBindVertexArray(0);
        //glDeleteVertexArrays(1,&ugh);
        return true;
    }

    void OpenGLShader::Enable()
    {
        glUseProgram(m_PID);
    }
    void OpenGLShader::Disable()
    {
        glUseProgram(0);
    }
};
