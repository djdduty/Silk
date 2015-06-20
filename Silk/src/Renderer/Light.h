#ifndef SILK_LIGHT_H
#define SILK_LIGHT_H

namespace Silk
{
    #define COLOR_WHITE Vec3(1.0f, 1.0f, 1.0f)
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
            Light(LightType Type) :
                m_Direction(Vec3()), m_Position(Vec3()), m_Color(COLOR_WHITE), m_Cutoff(0.0), m_Type(Type), m_Power(10.0)
            {
                m_Attenuation.Constant      = 0.0;
                m_Attenuation.Linear        = 0.0;
                m_Attenuation.Exponential   = 0.0;
            }
            ~Light() {}

            LightType m_Type;

            Vec3 m_Direction;
            Vec3 m_Position;
            Vec3 m_Color;
            f32 m_Cutoff; //this doesn't seem like a proper spotlight value

            f32 m_Power;

            struct
            {
                f32 Constant;
                f32 Linear;
                f32 Exponential;
            } m_Attenuation;
    };
}

#endif