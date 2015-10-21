#include <UIElements/TabPanel.h>
#include <UIElements/Common.h>
#include <InputManager.h>

namespace TestClient
{
    TabPanel::TabPanel(const Vec2& Size,InputManager* Input) : UIPanel(Size), m_Input(Input)
    {
        SetBackgroundColor(Vec4(0,0,0,0.75f));
        m_BorderSize = 2.0f ;
        m_TextSize   = 14.0f;
        m_SelectedTabIdx = 0;
    }
    TabPanel::~TabPanel()
    {
    }

    void TabPanel::OnMouseDown()
    {
        Vec2 cPos = m_Input->GetCursorPosition();
        bool TabIsClicked = false;
        for(i32 i = 0;i < m_Tabs.size();i++)
        {
            if(m_Tabs[i]->GetBounds()->Contains(cPos - GetAbsolutePosition().xy())) { TabIsClicked = true; break; }
        }
        if(!TabIsClicked) return;
        
        for(i32 i = 0;i < m_Tabs.size();i++)
        {
            m_TabViews[i]->SetEnabled(false);
            m_Tabs[i]->SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,0.9f));
            if(m_Tabs[i]->GetBounds()->Contains(cPos - GetAbsolutePosition().xy()))
            {
                m_Tabs[i]->SetBackgroundColor(Vec4(0.525f,0.525f,0.525f,0.9f));
                m_TabViews[i]->SetEnabled(true);
                m_SelectedTabIdx = i;
            }
        }
    }
    
    i32 TabPanel::AddTab(const string& TabName)
    {
        UIPanel* Tab = new UIPanel(Vec2(0,0));
        //m_Manager->AddElement(Tab );
        AddChild(Tab);
        
        UIPanel* View = new UIPanel(Vec2(0,0));
        //m_Manager->AddElement(View);
        AddChild(View);
        
        UIText * Label = AddText(m_Manager,m_TextSize,Vec2(2,2),TabName,Tab);
        //m_Manager->AddElement(Label);
        Tab->AddChild(Label);
        
        Tab ->SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,0.9f));
        View->SetBackgroundColor(Vec4(0.4f,0.4f,0.4f,0.95f));
        
        m_Tabs     .push_back(Tab  );
        m_TabViews .push_back(View );
        m_TabLabels.push_back(Label);
        
        UpdatePanels();
        return m_Tabs.size() - 1;
    }
    void TabPanel::UpdatePanels()
    {
        Scalar TabWidth   = (GetBounds()->GetDimensions().x - (2.0f * m_BorderSize) - (m_BorderSize * Scalar((m_Tabs.size() - 1)))) / Scalar(m_Tabs.size());
        Scalar TabHeight  = m_TextSize + (2.0f * m_BorderSize);
        Scalar ViewHeight = GetBounds()->GetDimensions().y - (TabHeight + (2.0f * m_BorderSize)) - m_BorderSize;
        
        for(i32 i = 0;i < m_Tabs.size();i++)
        {
            if(i != 0)
            {
                m_Tabs[i]->SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,0.9f));
                m_TabViews[i]->SetEnabled(false);
            }
            else
            {
                m_Tabs[i]->SetBackgroundColor(Vec4(0.525f,0.525f,0.525f,0.9f));
                m_TabViews[i]->SetEnabled(true);
            }
                
            //Divide space evenly among tabs
            m_Tabs[i]->SetSize(Vec2(TabWidth,TabHeight));
            m_Tabs[i]->SetPosition(Vec3(2.0f + (i * (TabWidth + m_BorderSize)),m_BorderSize,0.0f));
            
            //Place view under tabs
            m_TabViews[i]->SetSize(Vec2(GetBounds()->GetDimensions().x - (2.0f * m_BorderSize),ViewHeight));
            m_TabViews[i]->SetPosition(Vec3(m_BorderSize,(2.0f * m_BorderSize) + TabHeight,0.0f));
            
            //Center label
            m_TabLabels[i]->SetTextSize(m_TextSize);
            m_TabLabels[i]->RebuildMesh();
            Vec2 d = m_TabLabels[i]->GetBounds()->GetDimensions();
            m_TabLabels[i]->SetPosition(Vec3((TabWidth  * 0.5f) - (d.x * 0.5f),
                                             (TabHeight * 0.5f) - (d.y * 0.5f),0.0f));
        }
    }
};
