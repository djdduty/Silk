#include <UIElements/ListBoxPanel.h>
#include <UIElements/Common.h>
#include <InputManager.h>

namespace TestClient
{
    ListBoxPanel::ListBoxPanel(const Vec2& Size,InputManager* Input,UIElement* Parent) : UIPanel(Size), m_Input(Input)
    {
        SetBackgroundColor(Vec4(0,0,0,0.75f));
        m_BorderSize = 2.0f ;
        m_TextSize   = 14.0f;
        m_TextAlignment = TA_LEFT;
        m_ScrollBarSize = 12.0f;
        m_ScrollOffset = 0.0f;
        m_SelectionIndex = -1;
        m_Receiver = 0;
        Parent->AddChild(this);
        
        m_Scroll = new ScrollBar(Vec2(m_ScrollBarSize,Size.y - (2.0f * m_BorderSize)),Input,this);
        m_Scroll->SetReceiver(this);
    }
    ListBoxPanel::~ListBoxPanel()
    {
    }

    void ListBoxPanel::OnMouseDown()
    {
        if(!m_Receiver) return;
        Vec2 cPos = m_Input->GetCursorPosition();
        
        for(i32 i = 0;i < m_Entries.size();i++)
        {
            if(m_Entries[i]->GetBounds()->Contains(cPos - GetAbsolutePosition().xy()))
            {
                m_Receiver->OnListSelectionChanged(i,m_EntryLabels[i]->GetText(),this);
                m_SelectionIndex = i;
                return;
            }
        }
    }
    void ListBoxPanel::OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender)
    {
        Scalar ContentMinY   = m_Entries[0]->GetPosition().y;
        Scalar ContentMaxY   = m_Entries[m_Entries.size() - 1]->GetPosition().y + m_Entries[m_Entries.size() - 1]->GetBounds()->GetDimensions().y;
        Scalar ContentHeight = ContentMaxY - ContentMinY;
        Scalar MaxTrans      = ContentHeight - m_Bounds->GetDimensions().y;
        m_ScrollOffset       = -MaxTrans * CurrentValue;
        UpdatePanels();
    }
    
    i32 ListBoxPanel::AddEntry(const string& Name)
    {
        UIPanel* EntryPanel = new UIPanel(Vec2(0,0));
        EntryPanel->SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,0.9f));
        EntryPanel->EnableScissor(true);
        AddChild(EntryPanel);
        
        UIText* Label = AddText(m_Manager,m_TextSize,Vec2(2,2),Name,EntryPanel);
        
        m_Entries    .push_back(EntryPanel);
        m_EntryLabels.push_back(Label     );
        
        UpdatePanels();
        return m_Entries.size() - 1;
    }
    void ListBoxPanel::UpdatePanels()
    {
        m_Scroll->SetPosition(Vec3(m_Bounds->GetDimensions().x - m_BorderSize - m_ScrollBarSize,m_BorderSize,0.0f));
        m_Scroll->SetSize(Vec2(m_ScrollBarSize,m_Bounds->GetDimensions().y - (2.0f * m_BorderSize)));
        
        Scalar EntryWidth  = (m_Bounds->GetDimensions().x - (m_BorderSize * 2.0f)) - (m_ScrollBarSize + m_BorderSize);
        Scalar EntryHeight = m_TextSize + (2.0f * m_BorderSize);
        
        for(i32 i = 0;i < m_Entries.size();i++)
        {
            m_Entries[i]->SetSize(Vec2(EntryWidth,EntryHeight));
            m_Entries[i]->SetPosition(Vec3(m_BorderSize,(m_BorderSize + (i * (EntryHeight + m_BorderSize))) + m_ScrollOffset,0.0f));
            
            if(i == m_SelectionIndex) m_Entries[i]->SetBackgroundColor(Vec4(0.5f,0.5f,0.5f,0.9f));
            else m_Entries[i]->SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,0.9f));
            
            //Center label
            m_EntryLabels[i]->SetTextSize(m_TextSize);
            m_EntryLabels[i]->RebuildMesh();
            
            Vec2 d = m_EntryLabels[i]->GetBounds()->GetDimensions();
            switch(m_TextAlignment)
            {
                case TA_LEFT:
                {
                    m_EntryLabels[i]->SetPosition(Vec3(m_BorderSize,(EntryHeight * 0.5f) - (d.y * 0.5f),0.0f));
                    break;
                }
                case TA_CENTER:
                {
                    m_EntryLabels[i]->SetPosition(Vec3((EntryWidth  * 0.5f) - (d.x * 0.5f),
                                                       (EntryHeight * 0.5f) - (d.y * 0.5f),0.0f));
                    break;
                }
                case TA_RIGHT:
                {
                    Vec2 d = m_EntryLabels[i]->GetBounds()->GetDimensions();
                    m_EntryLabels[i]->SetPosition(Vec3(EntryWidth - m_BorderSize - d.x,(EntryHeight * 0.5f) - (d.y * 0.5f),0.0f));
                    break;
                }
            }
        }
    }
};
