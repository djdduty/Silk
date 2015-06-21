#ifndef SILK_LIGHT_H
#define SILK_LIGHT_H

namespace Silk
{
    #define COLOR_WHITE Vec4(1.0f, 1.0f, 1.0f, 1.0f)
    enum LightType
    {
        LT_POINT,
        LT_SPOT,
        LT_DIRECTIONAL,
        LT_COUNT
    };

    class Light
    {
        public:
            Light() :
                m_Direction(Vec4()), m_Position(Vec4()), m_Color(COLOR_WHITE), m_Cutoff(0.0), m_Soften(1.0), m_Type(LT_POINT), m_Power(10.0)
            {
                m_Attenuation.Constant      = 0.0;
                m_Attenuation.Linear        = 0.0;
                m_Attenuation.Exponential   = 0.0;
            }
        
            Light(LightType Type) :
                m_Type(Type), m_Position(Vec4()), m_Direction(Vec4()), m_Color(COLOR_WHITE), m_Cutoff(0.0), m_Soften(1.0), m_Power(10.0)
            {
                m_Attenuation.Constant    = 0.0;
                m_Attenuation.Linear      = 0.0;
                m_Attenuation.Exponential = 0.0;
            }
            ~Light() {}

            Vec4 m_Position ;
            Vec4 m_Direction;
            Vec4 m_Color    ;
            f32 m_Cutoff    ; //this doesn't seem like a proper spotlight value
            f32 m_Soften    ;
            f32 m_Power     ;

            struct
            {
                f32 Constant   ;
                f32 Linear     ;
                f32 Exponential;
            } m_Attenuation;
        
            LightType m_Type;
            f32 _Padding; //Keep this data structure 16-byte aligned
    };
}

#endif