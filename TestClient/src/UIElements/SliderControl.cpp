#include <UIElements/SliderControl.h>

namespace TestClient
{
    void SliderControl::_Initialize()
    {
        if(!m_Manager)
            return;
        m_SliderStyle = new UIElementStyle(this);
        m_Background  = new UIRenderRectangle(m_Manager, GetStyle(UIS_DEFAULT));
        m_Slider      = new UIRenderRectangle(m_Manager, m_SliderStyle);
        GetCurrentStyle()->SetSize(Vec2(200, 5));
        GetCurrentStyle()->SetBackgroundColor(Vec4(0,0,1,1));
        m_SliderStyle->SetBackgroundColor(Vec4(0,1,0,1));
        m_SliderStyle->SetSize(Vec2(20,20));
        AddContent(m_Background);
        AddContent(m_Slider);
    }

    void SliderControl::Update(Scalar dt) 
    {
    
    }
    void SliderControl::OnUpdateTransforms()
    {
        f32 Height      = GetCurrentStyle()->GetSize().y;
        Vec2 SliderSize = m_SliderStyle->GetSize();
        m_SliderStyle->SetPosition(GetCurrentStyle()->GetPosition() + Vec3(GetSliderPosition(), (Height/2)-(SliderSize.y/2), -1));
    }
    void SliderControl::OnUpdateMaterials()
    {

    }
    void SliderControl::OnUpdateMeshes()
    {

    }
    void SliderControl::OnUpdateContent()
    {

    }
    void SliderControl::SetValue(f32 val)
    {
        if(val < m_Min || val > m_Max)
            return;
        m_Value = val;
    }

    f32 SliderControl::GetSliderPosition()
    {
        f32 total = m_Max - m_Min;
        f32 deltaVal = m_Value - m_Min;
        f32 percent = m_Value / m_Max;
        f32 offset = GetCurrentStyle()->GetSize().x * percent;
        offset -= (m_SliderStyle->GetSize().x*0.5);
        return offset;
    }
}