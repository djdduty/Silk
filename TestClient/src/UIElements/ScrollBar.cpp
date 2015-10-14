#include <UIElements/ScrollBar.h>
#include <UIElements/Common.h>
#include <InputManager.h>

namespace TestClient
{
    ScrollBar::ScrollBar(const Vec2& Size,InputManager* Input,UIElement* Parent) : UIPanel(Size), m_Input(Input)
    {
        SetBackgroundColor(Vec4(0,0,0,0.85f));
        m_BorderSize = 2.0f;
        m_Receiver   = 0;
        m_BarClicked = false;
        m_BarOffset  = 0.0f;
        m_BarFactor  = m_LastBarFac = 0.0f;
        m_BarHeight  = Size.x;
        
        Parent->AddChild(this);
        
        m_Bar = new UIPanel(Vec2(Size.x - (2.0f * m_BorderSize),m_BarHeight));
        m_Bar->SetBackgroundColor(Vec4(0.25f,0.25f,0.25f,0.95f));
        AddChild(m_Bar);
        
        UpdatePanels();
    }
    ScrollBar::~ScrollBar()
    {
    }

    void ScrollBar::OnMouseDown()
    {
        Vec2 cPos = m_Input->GetCursorPosition();
        if(m_Bar->GetBounds()->Contains(cPos - GetAbsolutePosition().xy())) m_BarClicked = true;
    }
    void ScrollBar::OnMouseUp() { m_BarClicked = false; }
    void ScrollBar::OnMouseMove()
    {
        if(!m_BarClicked) return;
        Scalar delta = m_Input->GetCursorDelta().y;
        
        Scalar tmp = m_BarOffset;
        
        m_BarOffset += delta;
        
        if(m_BarOffset < 0) m_BarOffset = 0;
        else if(m_BarOffset > m_Bounds->GetDimensions().y - m_BarHeight - m_BorderSize)
        { m_BarOffset = m_Bounds->GetDimensions().y - m_BarHeight - m_BorderSize; }
        
        m_LastBarFac = m_BarFactor;
        m_BarFactor  = m_BarOffset / (m_Bounds->GetDimensions().y - m_BarHeight - m_BorderSize);
        
        if(m_BarOffset != tmp)
        {
            UpdatePanels();
            if(m_Receiver) m_Receiver->OnScrollChanged(m_BarFactor,m_BarFactor - m_LastBarFac,this);
        }
    }
    void ScrollBar::UpdatePanels()
    {
        m_Bar->SetSize(Vec2(m_Bounds->GetDimensions().x - (2.0f * m_BorderSize),m_BarHeight));
        m_Bar->SetPosition(Vec3(m_BorderSize,m_BorderSize + m_BarOffset,0.0f));
    }
};
