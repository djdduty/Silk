#include <Raster/Raster.h>
#include <Raster/OpenGL/OpenGLUniform.h>
#include <Raster/OpenGL/PlatformSpecificOpenGL.h>
#include <Utilities/Utilities.h>
#include <math.h>

namespace Silk
{
    void UniformCall::AcquireLocations(u32 PID)
    {
        m_Location = glGetUniformLocation(PID,m_Def->Name.c_str());
    }

    #define UC_Calls(C,P1,P2,F1,F2)                     \
    void C::Call     (void* Data)                       \
    {                                                   \
        if(m_Location == -1) return;                    \
        F1(m_Location,P1 Data);                         \
    }                                                   \
    void C::CallArray(void* Data)                       \
    {                                                   \
        if(m_Location == -1) return;                    \
        F2(m_Location,m_Def->ArraySize,P2 Data);        \
    }
    
    UC_Calls(UC_Bool  ,*(i32 *),(i32 *),glUniform1i ,glUniform1iv );
    UC_Calls(UC_Int   ,*(i32 *),(i32 *),glUniform1i ,glUniform1iv );
    UC_Calls(UC_UInt  ,*(u32 *),(u32 *),glUniform1ui,glUniform1uiv);
    UC_Calls(UC_Float ,*(f32 *),(f32 *),glUniform1f ,glUniform1fv );
    UC_Calls(UC_Double,*(f64 *),(f64 *),glUniform1d ,glUniform1dv );
    
    #undef UC_Calls
    
    #define UC_Calls(C,F1,F2)                           \
    void C::Call     (void* Data)                       \
    {                                                   \
        if(m_Location == -1) return;                    \
        F1;                                             \
    }                                                   \
    void C::CallArray(void* Data)                       \
    {                                                   \
        if(m_Location == -1) return;                    \
        F2;                                             \
    }
    UC_Calls(UC_Vec2,glUniform2f (m_Location,(*(Vec2*)Data).x,(*(Vec2*)Data).y),
                     glUniform2fv(m_Location,m_Def->ArraySize,(f32*)Data));
    UC_Calls(UC_Vec3,glUniform3f (m_Location,(*(Vec2*)Data).x,(*(Vec3*)Data).y,(*(Vec3*)Data).z),
                     glUniform3fv(m_Location,m_Def->ArraySize,(f32*)Data));
    UC_Calls(UC_Vec4,glUniform4f (m_Location,(*(Vec4*)Data).x,(*(Vec4*)Data).y,(*(Vec4*)Data).z,(*(Vec4*)Data).w),
                     glUniform4fv(m_Location,m_Def->ArraySize,(f32*)Data));
    
    #undef UC_Calls
    
    void UC_Mat4::Call(void *Data)
    {
        if(m_Location == -1) return;
        glUniformMatrix4fv(m_Location,1,GL_TRUE,(f32*)Data);
    }
    void UC_Mat4::CallArray(void *Data)
    {
        if(m_Location == -1) return;
        glUniformMatrix4fv(m_Location,m_Def->ArraySize,GL_TRUE,(f32*)Data);
    }
    
    UC_Light::~UC_Light()
    {
        if(m_PositionLocs   ) delete [] m_PositionLocs   ;
        if(m_DirectionLocs  ) delete [] m_DirectionLocs  ;
        if(m_ColorLocs      ) delete [] m_ColorLocs      ;
        if(m_CutoffLocs     ) delete [] m_CutoffLocs     ;
        if(m_SoftenLocs     ) delete [] m_SoftenLocs     ;
        if(m_PowerLocs      ) delete [] m_PowerLocs      ;
        if(m_ConstantLocs   ) delete [] m_ConstantLocs   ;
        if(m_LinearLocs     ) delete [] m_LinearLocs     ;
        if(m_ExponentialLocs) delete [] m_ExponentialLocs;
        if(m_TypeLocs       ) delete [] m_TypeLocs       ;
    }
    void UC_Light::Call(void *Data)
    {
        if(!m_PositionLocs || m_PositionLocs[0] == -1) return;
        
        Light* Lt = (Light*)Data;
        if(m_PositionLocs   [0] != -1) glUniform4f(m_PositionLocs   [0],Lt->m_Position .x,Lt->m_Position .y,Lt->m_Position .z,Lt->m_Position .w);
        if(m_DirectionLocs  [0] != -1) glUniform4f(m_DirectionLocs  [0],Lt->m_Direction.x,Lt->m_Direction.y,Lt->m_Direction.z,Lt->m_Direction.w);
        if(m_ColorLocs      [0] != -1) glUniform4f(m_ColorLocs      [0],Lt->m_Color    .x,Lt->m_Color    .y,Lt->m_Color    .z,Lt->m_Color    .w);
        if(m_CutoffLocs     [0] != -1) glUniform1f(m_CutoffLocs     [0],cos(Lt->m_Cutoff * PI_OVER_180                                        ));
        if(m_SoftenLocs     [0] != -1) glUniform1f(m_SoftenLocs     [0],cos(((1.0f - Lt->m_Soften) * Lt->m_Cutoff) * PI_OVER_180              ));
        if(m_PowerLocs      [0] != -1) glUniform1f(m_PowerLocs      [0],Lt->m_Power                                                            );
        if(m_ConstantLocs   [0] != -1) glUniform1f(m_ConstantLocs   [0],Lt->m_Attenuation.Constant                                             );
        if(m_LinearLocs     [0] != -1) glUniform1f(m_LinearLocs     [0],Lt->m_Attenuation.Linear                                               );
        if(m_ExponentialLocs[0] != -1) glUniform1f(m_ExponentialLocs[0],Lt->m_Attenuation.Exponential                                          );
        if(m_TypeLocs       [0] != -1) glUniform1i(m_TypeLocs       [0],Lt->m_Type                                                             );
    }
    void UC_Light::CallArray(void *Data)
    {
        if(!m_PositionLocs || m_PositionLocs[0] == -1) return;
        
        for(i32 i = 0;i < m_Def->ArraySize;i++)
        {
            Light* Lt = ((Light**)Data)[i];
            
            if(m_PositionLocs   [i] != -1) glUniform4f(m_PositionLocs   [i],Lt->m_Position .x,Lt->m_Position .y,Lt->m_Position .z,Lt->m_Position .w);
            if(m_DirectionLocs  [i] != -1) glUniform4f(m_DirectionLocs  [i],Lt->m_Direction.x,Lt->m_Direction.y,Lt->m_Direction.z,Lt->m_Direction.w);
            if(m_ColorLocs      [i] != -1) glUniform4f(m_ColorLocs      [i],Lt->m_Color    .x,Lt->m_Color    .y,Lt->m_Color    .z,Lt->m_Color    .w);
            if(m_CutoffLocs     [i] != -1) glUniform1f(m_CutoffLocs     [i],cos(Lt->m_Cutoff * PI_OVER_180                                        ));
            if(m_SoftenLocs     [i] != -1) glUniform1f(m_SoftenLocs     [i],cos(((1.0f - Lt->m_Soften) * Lt->m_Cutoff) * PI_OVER_180              ));
            if(m_PowerLocs      [i] != -1) glUniform1f(m_PowerLocs      [i],Lt->m_Power                                                            );
            if(m_ConstantLocs   [i] != -1) glUniform1f(m_ConstantLocs   [i],Lt->m_Attenuation.Constant                                             );
            if(m_LinearLocs     [i] != -1) glUniform1f(m_LinearLocs     [i],Lt->m_Attenuation.Linear                                               );
            if(m_ExponentialLocs[i] != -1) glUniform1f(m_ExponentialLocs[i],Lt->m_Attenuation.Exponential                                          );
            if(m_TypeLocs       [i] != -1) glUniform1i(m_TypeLocs       [i],Lt->m_Type                                                             );
        }
    }
    void UC_Light::AcquireLocations(u32 PID)
    {
        i32 ArraySize = m_Def->ArraySize;
        if(ArraySize == -1) ArraySize = 1;
        
        m_PositionLocs    = new i32[ArraySize];
        m_DirectionLocs   = new i32[ArraySize];
        m_ColorLocs       = new i32[ArraySize];
        m_CutoffLocs      = new i32[ArraySize];
        m_SoftenLocs      = new i32[ArraySize];
        m_PowerLocs       = new i32[ArraySize];
        m_ConstantLocs    = new i32[ArraySize];
        m_LinearLocs      = new i32[ArraySize];
        m_ExponentialLocs = new i32[ArraySize];
        m_TypeLocs        = new i32[ArraySize];
        
        if(m_Def->ArraySize != -1)
        {
            for(i32 i = 0;i < m_Def->ArraySize;i++)
            {
                m_PositionLocs   [i] = glGetUniformLocation(PID,FormatString("%s[%d].Position"    ,m_Def->Name.c_str(),i).c_str());
                m_DirectionLocs  [i] = glGetUniformLocation(PID,FormatString("%s[%d].Direction"   ,m_Def->Name.c_str(),i).c_str());
                m_ColorLocs      [i] = glGetUniformLocation(PID,FormatString("%s[%d].Color"       ,m_Def->Name.c_str(),i).c_str());
                m_CutoffLocs     [i] = glGetUniformLocation(PID,FormatString("%s[%d].Cutoff"      ,m_Def->Name.c_str(),i).c_str());
                m_SoftenLocs     [i] = glGetUniformLocation(PID,FormatString("%s[%d].Soften"      ,m_Def->Name.c_str(),i).c_str());
                m_PowerLocs      [i] = glGetUniformLocation(PID,FormatString("%s[%d].Power"       ,m_Def->Name.c_str(),i).c_str());
                m_ConstantLocs   [i] = glGetUniformLocation(PID,FormatString("%s[%d].CAtt"        ,m_Def->Name.c_str(),i).c_str());
                m_LinearLocs     [i] = glGetUniformLocation(PID,FormatString("%s[%d].LAtt"        ,m_Def->Name.c_str(),i).c_str());
                m_ExponentialLocs[i] = glGetUniformLocation(PID,FormatString("%s[%d].QAtt"        ,m_Def->Name.c_str(),i).c_str());
                m_TypeLocs       [i] = glGetUniformLocation(PID,FormatString("%s[%d].Type"        ,m_Def->Name.c_str(),i).c_str());
            }
        }
        else
        {
            m_PositionLocs   [0] = glGetUniformLocation(PID,FormatString("%s.Position"    ,m_Def->Name.c_str()).c_str());
            m_DirectionLocs  [0] = glGetUniformLocation(PID,FormatString("%s.Direction"   ,m_Def->Name.c_str()).c_str());
            m_ColorLocs      [0] = glGetUniformLocation(PID,FormatString("%s.Color"       ,m_Def->Name.c_str()).c_str());
            m_CutoffLocs     [0] = glGetUniformLocation(PID,FormatString("%s.Cutoff"      ,m_Def->Name.c_str()).c_str());
            m_SoftenLocs     [0] = glGetUniformLocation(PID,FormatString("%s.Soften"      ,m_Def->Name.c_str()).c_str());
            m_PowerLocs      [0] = glGetUniformLocation(PID,FormatString("%s.Power"       ,m_Def->Name.c_str()).c_str());
            m_ConstantLocs   [0] = glGetUniformLocation(PID,FormatString("%s.CAtt"        ,m_Def->Name.c_str()).c_str());
            m_LinearLocs     [0] = glGetUniformLocation(PID,FormatString("%s.LAtt"        ,m_Def->Name.c_str()).c_str());
            m_ExponentialLocs[0] = glGetUniformLocation(PID,FormatString("%s.QAtt"        ,m_Def->Name.c_str()).c_str());
            m_TypeLocs       [0] = glGetUniformLocation(PID,FormatString("%s.Type"        ,m_Def->Name.c_str()).c_str());
        }
    }
};
