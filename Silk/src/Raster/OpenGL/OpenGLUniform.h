#pragma once
#include <System/SilkTypes.h>

namespace Silk
{
    class UniformDef;
    class UniformCall
    {
        public:
            UniformCall(UniformDef* Def) : m_Location(-1), m_Def(Def) { }
            virtual ~UniformCall() { }
        
            virtual void Call     (void* Data) = 0;
            virtual void CallArray(void* Data) = 0;
            virtual void AcquireLocations(u32 PID);
            virtual UniformCall* Factory() = 0;
            
            i32 m_Location;
            
            UniformDef* m_Def;
    };
    
    #define UC_Class(C)                                             \
    class C : public UniformCall                                    \
    {                                                               \
        public:                                                     \
             C(UniformDef* Def) : UniformCall(Def) { }              \
            ~C() { }                                                \
                                                                    \
            virtual void Call     (void* Data);                     \
            virtual void CallArray(void* Data);                     \
            virtual UniformCall* Factory() { return new C(m_Def); } \
    }
    
    UC_Class(UC_Bool);
    UC_Class(UC_Int);
    UC_Class(UC_UInt);
    UC_Class(UC_Float);
    UC_Class(UC_Double);
    UC_Class(UC_Vec2);
    UC_Class(UC_Vec3);
    UC_Class(UC_Vec4);
    UC_Class(UC_Mat4);
    
    class UC_Light : public UniformCall
    {
        public:
            UC_Light(UniformDef* Def) : UniformCall(Def), m_PositionLocs(0), m_DirectionLocs(0),
                m_ColorLocs(0), m_CutoffLocs(0), m_SoftenLocs(0), m_PowerLocs(0), m_ConstantLocs(0),
                m_LinearLocs(0), m_ExponentialLocs(0), m_TypeLocs(0) { }
            ~UC_Light();
        
            virtual void Call     (void* Data);
            virtual void CallArray(void* Data);
            virtual void AcquireLocations(u32 PID);
            virtual UniformCall* Factory() { return new UC_Light(m_Def); }
        
            i32* m_PositionLocs   ;
            i32* m_DirectionLocs  ;
            i32* m_ColorLocs      ;
            i32* m_CutoffLocs     ;
            i32* m_SoftenLocs     ;
            i32* m_PowerLocs      ;
            i32* m_ConstantLocs   ;
            i32* m_LinearLocs     ;
            i32* m_ExponentialLocs;
            i32* m_TypeLocs       ;
    };
    
    #undef UC_Class
};
