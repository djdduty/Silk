#ifndef SILK_SLIDERCONTROL_H
#define SILK_SLIDERCONTROL_H

#include <UI/UIElement.h>

using namespace Silk;
namespace TestClient
{
    class SliderControl : public UIElement
    {
        public:
            SliderControl(f32 min, f32 max) : m_Min(min), m_Max(max), m_Value((max*0.5)+(min*0.5)) {}
            ~SliderControl() { delete m_Slider; delete m_SliderStyle; }

            void SetValue(f32 Val);
            UIElementStyle* GetSliderStyle() { return m_SliderStyle; }

            void Update(Scalar dt);
            void UpdateTransforms();
            void UpdateMaterials();
            void UpdateMeshes();
            void UpdateContent();
            void Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);

        protected:
            friend class UIManager;

            UIRenderRectangle* m_Slider;
            UIElementStyle* m_SliderStyle;

            void _Initialize();

            f32 m_Min;
            f32 m_Max;
            f32 m_Value;

            f32 GetSliderPosition();
    };
};

#endif