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
        GetCurrentStyle()->SetBackgroundColor(Vec4(90,90,90,255).Normalized());
        m_SliderStyle->SetBackgroundColor(Vec4(120,120,120,255).Normalized());
        m_SliderStyle->SetSize(Vec2(20,20));
        m_SliderTextStyle = new UIElementStyle(this);
        m_SliderLabel = new UIText(m_Manager, m_SliderTextStyle);
        m_SliderTextStyle->SetTextColor(Vec4(1,1,1,1));
        m_SliderLabel->SetText("Test");
        AddContent(m_Background);
        AddContent(m_Slider);
        AddContent(m_SliderLabel);
    }

    void SliderControl::Update(Scalar dt) 
    {
        //SetValue(50);
    }
    void SliderControl::OnUpdateTransforms()
    {
        f32 Height      = GetCurrentStyle()->GetSize().y;
        Vec2 SliderSize = m_SliderStyle->GetSize();
        Vec3 Position   = GetCurrentStyle()->GetPosition();
        Vec2 TextSize   = m_SliderLabel->GetSize();
        m_SliderStyle->SetPosition(Position + Vec3(GetSliderPosition() - (SliderSize.x*0.5), (Height * 0.5)-(SliderSize.y * 0.5), -1));
        m_SliderTextStyle->SetPosition(Position + Vec3(GetSliderPosition() - (TextSize.x * 0.5), (Height * 0.5)-(TextSize.y * 0.5), -2));
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
        
        //m_SliderLabel->SetText(to_string(val)); TODO: We have no numbers in the font atm
    }

    f32 SliderControl::GetSliderPosition()
    {
        f32 total = m_Max - m_Min;
        f32 deltaVal = m_Value - m_Min;
        f32 percent = m_Value / m_Max;
        f32 offset = GetCurrentStyle()->GetSize().x * percent;
        //offset -= (m_SliderStyle->GetSize().x*0.5);
        return offset;
    }
}