#ifndef SILK_SLIDERCONTROL_H
#define SILK_SLIDERCONTROL_H

#include <UI/UIElement.h>
#include <UI/UIText.h>

using namespace Silk;
namespace TestClient
{
    class SliderControl : public UIElement
    {
        public:
            SliderControl(f32 min, f32 max) : m_Min(min), m_Max(max), m_Value((max*0.5)+(min*0.5)) {}
            ~SliderControl() { delete m_Background; delete m_Slider; delete m_SliderStyle; delete m_SliderLabel; delete m_SliderTextStyle; }

            void SetValue(f32 Val);
            UIElementStyle* GetSliderStyle() { return m_SliderStyle; }

            void Update(Scalar dt);
            void OnUpdateTransforms();
            void OnUpdateMaterials();
            void OnUpdateMeshes();
            void OnUpdateContent();

        protected:
            friend class UIManager;

            UIRenderRectangle* m_Slider;
            UIRenderRectangle* m_Background;
            UIText*         m_SliderLabel;
            UIElementStyle* m_SliderStyle;
            UIElementStyle* m_SliderTextStyle;

            void _Initialize();

            f32 m_Min;
            f32 m_Max;
            f32 m_Value;

            f32 GetSliderPosition();
    };
};

#endif