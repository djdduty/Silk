#include <UIElements/SliderControl.h>

namespace TestClient
{
    void SliderControl::_Initialize()
    {
        if(!m_Manager)
            return;
        m_Background  = new UIRenderRectangle(m_Manager);
        m_Slider      = new UIRenderRectangle(m_Manager);
        m_SliderStyle = new UIElementStyle(this);
        GetCurrentStyle()->SetSize(Vec2(200, 5));
        GetCurrentStyle()->SetBackgroundColor(Vec4(0,0,1,1));
        m_SliderStyle->SetBackgroundColor(Vec4(0,1,0,1));
        m_SliderStyle->SetSize(Vec2(20,20));
    }

    void SliderControl::Update(Scalar dt) {}

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

    void SliderControl::UpdateTransforms()
    {
        m_Background->UpdateTransform(GetCurrentStyle());
        m_SliderStyle->SetPosition(GetCurrentStyle()->GetPosition()+Vec3(GetSliderPosition(), -7.5, -1));
        m_Slider->UpdateTransform(m_SliderStyle);
    }

    void SliderControl::UpdateMaterials()
    {
        m_Background->UpdateMaterial(GetCurrentStyle());
        m_Slider->UpdateMaterial(m_SliderStyle);
    }

    void SliderControl::UpdateMeshes()
    {
        m_Background->UpdateMesh(GetCurrentStyle());
        m_Slider->UpdateMesh(m_SliderStyle);
    }

    void SliderControl::UpdateContent()
    {}

    void SliderControl::Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered)
    {
        m_Slider->Render(PrimType, ObjectsRendered);
        m_Background->Render(PrimType, ObjectsRendered);
    }
}