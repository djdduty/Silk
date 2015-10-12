/*
 *  TablePanel.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 10/5/15.
 *
 */

#include <UIElements/TablePanel.h>
#include <UIElements/Common.h>
#include <InputManager.h>

namespace TestClient
{
    TablePanel::TablePanel(UIManager* Mgr,const Vec2& Size,InputManager* Input) : UIPanel(Size), m_Input(Input)
    {
        SetBackgroundColor(Vec4(0,0,0,0.75f));
        m_Manager = Mgr;
        m_BorderSize = 2.0f ;
        m_TextSize   = 14.0f;
        m_SepFac     = 0.75f;
        m_MinSepFac  = 0.0f;
        m_MaxSepFac  = 1.0f;
        m_TableValueBorderSize = 1.0f;
        m_SeparatorClicked = false;
        m_CenterValueNames = false;
        m_CenterValues = false;
        
        m_NamePanel = new UIPanel(Vec2((Size.x - (3.0f * m_BorderSize)) / 2.0f,m_TextSize + (m_BorderSize * 2.0f)));
        m_NamePanel->SetBackgroundColor(Vec4(0.575f,0.575f,0.575f,0.9f));
        AddChild(m_NamePanel);
        m_Name = AddText(Mgr,m_TextSize * 1.2f,Vec2(m_BorderSize,m_BorderSize),"Name",m_NamePanel);
        
        m_ValuePanel = new UIPanel(Vec2((Size.x - (3.0f * m_BorderSize)) / 2.0f,m_TextSize + (m_BorderSize * 2.0f)));
        m_ValuePanel->SetBackgroundColor(Vec4(0.575f,0.575f,0.575f,0.9f));
        AddChild(m_ValuePanel);
        m_Value = AddText(Mgr,m_TextSize * 1.2f,Vec2(m_BorderSize,m_BorderSize),"Value",m_ValuePanel);
        
        m_Separator = new UIPanel(Vec2(m_BorderSize * 2.0f,Size.y));
        AddChild(m_Separator);
        
        UpdatePanels();
    }
    TablePanel::~TablePanel()
    {
    }

    void TablePanel::OnMouseDown()
    {
        Vec2 cPos = m_Input->GetCursorPosition();
        if(m_Separator->GetBounds()->Contains((cPos - GetAbsolutePosition().xy()) + Vec2(m_BorderSize,0.0f))) m_SeparatorClicked = true;
    }
    void TablePanel::OnMouseMove()
    {
        if(m_SeparatorClicked)
        {
            Vec2 d = m_Input->GetCursorDelta();
            Scalar Pcnt = d.x / m_Bounds->GetDimensions().x;
            m_SepFac += Pcnt * 0.5f;
            if(m_SepFac < m_MinSepFac) m_SepFac = m_MinSepFac;
            else if(m_SepFac > m_MaxSepFac) m_SepFac = m_MaxSepFac;
            
            UpdatePanels();
        }
    }
    void TablePanel::OnMouseUp()
    {
        m_SeparatorClicked = false;
    }
    void TablePanel::Clear()
    {
        for(i32 i = 0;i < m_Values.size();i++)
        {
            m_ValueNamePanels[i]->RemoveChild(m_ValueNames[i]); m_ValueNames[i]->Destroy();
            m_ValueDispPanels[i]->RemoveChild(m_ValueDisps[i]); m_ValueDisps[i]->Destroy();
            RemoveChild(m_ValueNamePanels[i]); m_ValueNamePanels[i]->Destroy();
            RemoveChild(m_ValueDispPanels[i]); m_ValueDispPanels[i]->Destroy();
            delete m_Values[i];
        }
        m_ValueNamePanels.clear();
        m_ValueDispPanels.clear();
        m_ValueNames.clear();
        m_ValueDisps.clear();
        m_Values.clear();
        UpdatePanels();
    }
    void TablePanel::SetMinSeparatorFactor(Scalar Fac)
    {
        if(Fac < 0.0f) Fac = 0.0f;
        if(Fac > 1.0f) Fac = 1.0f;
        
        if(m_MaxSepFac < Fac) m_MaxSepFac = Fac;
        m_MinSepFac = Fac;
        UpdatePanels();
    }
    void TablePanel::SetMaxSeparatorFactor(Scalar Fac)
    {
        if(Fac < 0.0f) Fac = 0.0f;
        if(Fac > 1.0f) Fac = 1.0f;
        
        if(m_MinSepFac > Fac) m_MinSepFac = Fac;
        m_MaxSepFac = Fac;
        UpdatePanels();
    }
    void TablePanel::SetCenterValueNames(bool Flag)
    {
        m_CenterValueNames = Flag;
        UpdatePanels();
    }
    void TablePanel::SetCenterValues(bool Flag)
    {
        m_CenterValues = Flag;
        UpdatePanels();
    }
    i32 TablePanel::AddValue(const string &Name,ConfigValue *Val)
    {
        UIPanel* np = new UIPanel(Vec2(0,0));
        np->SetBackgroundColor(Vec4(0.4f,0.4f,0.4f,1.0f));
        AddChild(np);
        
        UIPanel* vp = new UIPanel(Vec2(0,0));
        vp->SetBackgroundColor(Vec4(0.4f,0.4f,0.4f,1.0f));
        AddChild(vp);
        
        m_ValueNamePanels.push_back(np);
        m_ValueDispPanels.push_back(vp);
        m_ValueNames     .push_back(AddText(m_Manager,m_TextSize,Vec2(0,0),Name,np));
        m_ValueDisps     .push_back(AddText(m_Manager,m_TextSize,Vec2(0,0),""  ,vp));
        
        m_Values.push_back(Val);
        
        UpdatePanels();
        return m_Values.size() - 1;
    }
    void TablePanel::UpdatePanels()
    {
        Scalar SepPos = (m_SepFac * m_Bounds->GetDimensions().x);
        Scalar SepWidth = m_BorderSize * 2.0f;
        Scalar NameWidth  = SepPos - m_BorderSize - (0.5f * SepWidth);
        Scalar ValueWidth = (m_Bounds->GetDimensions().x - SepPos) - m_BorderSize - (0.5f * SepWidth);
        
        m_NamePanel ->SetSize(Vec2(NameWidth ,m_TextSize + (m_BorderSize * 2.0f)));
        m_NamePanel ->SetPosition(Vec3(m_BorderSize,m_BorderSize,0.0f));
        
        m_ValuePanel->SetSize(Vec2(ValueWidth,m_TextSize + (m_BorderSize * 2.0f)));
        m_ValuePanel->SetPosition(Vec3(SepPos + (0.5f * SepWidth),m_BorderSize,0.0f));
        
        m_Separator->SetSize(Vec2(SepWidth,m_Bounds->GetDimensions().y));
        m_Separator->SetPosition(Vec3(SepPos - (m_BorderSize * 0.5f),0.0f,0.0f));
        
        m_Name->SetTextSize(m_TextSize * 1.2f);
        m_Name->RebuildMesh();
        m_Name ->SetPosition(Vec3((m_NamePanel->GetBounds()->GetDimensions().x * 0.5f) - (m_Name->GetBounds()->GetDimensions().x * 0.5f),
                                  (m_NamePanel->GetBounds()->GetDimensions().y * 0.5f) - (m_Name->GetBounds()->GetDimensions().y * 0.5f),0.0f));
        
        m_Value->SetTextSize(m_TextSize * 1.2f);
        m_Value->RebuildMesh();
        m_Value->SetPosition(Vec3((m_ValuePanel->GetBounds()->GetDimensions().x * 0.5f) - (m_Value->GetBounds()->GetDimensions().x * 0.5f),
                                  (m_ValuePanel->GetBounds()->GetDimensions().y * 0.5f) - (m_Value->GetBounds()->GetDimensions().y * 0.5f),0.0f));
        
        Vec2 npSize = m_NamePanel ->GetBounds()->GetDimensions();
        Vec2 vpSize = m_ValuePanel->GetBounds()->GetDimensions();
        for(i32 i = 0;i < m_Values.size();i++)
        {
            m_ValueNamePanels[i]->SetSize(npSize);
            m_ValueNamePanels[i]->SetPosition(m_NamePanel->GetPosition() + Vec3(0,(npSize.y + m_TableValueBorderSize) * (i + 1),0.0f));
            
            m_ValueDispPanels[i]->SetSize(vpSize);
            m_ValueDispPanels[i]->SetPosition(m_ValuePanel->GetPosition() + Vec3(0,(npSize.y + m_TableValueBorderSize) * (i + 1),0.0f));
            
            string Str;
            switch(m_Values[i]->GetType())
            {
                case ConfigValue::VT_BYTE:
                {
                    Byte v = *m_Values[i];
                    Str = FormatString("%d",(i8)v);
                    break;
                }
                case ConfigValue::VT_UBYTE:
                {
                    Byte v = *m_Values[i];
                    Str = FormatString("%d",(u8)v);
                    break;
                }
                case ConfigValue::VT_I16:
                {
                    i16 v = *m_Values[i];
                    Str = FormatString("%d",v);
                    break;
                }
                case ConfigValue::VT_U16:
                {
                    u16 v = *m_Values[i];
                    Str = FormatString("%d",v);
                    break;
                }
                case ConfigValue::VT_I32:
                {
                    i32 v = *m_Values[i];
                    Str = FormatString("%d",v);
                    break;
                }
                case ConfigValue::VT_U32:
                {
                    u32 v = *m_Values[i];
                    Str = FormatString("%d",v);
                    break;
                }
                case ConfigValue::VT_F32:
                {
                    f32 v = *m_Values[i];
                    Str = FormatString("%f",v);
                    break;
                }
                case ConfigValue::VT_VEC2:
                {
                    Vec2 v = *m_Values[i];
                    Str = FormatString("%f,%f,%f,%f",v.x,v.y);
                    break;
                }
                case ConfigValue::VT_VEC3:
                {
                    Vec3 v = *m_Values[i];
                    Str = FormatString("%f,%f,%f",v.x,v.y,v.z);
                    break;
                }
                case ConfigValue::VT_VEC4:
                {
                    Vec4 v = *m_Values[i];
                    Str = FormatString("%f,%f,%f,%f",v.x,v.y,v.z,v.w);
                    break;
                }
                case ConfigValue::VT_STRING:
                {
                    Str = (string)*m_Values[i];
                    break;
                }
                default:
                {
                    printf("Invalid value type (%s).\n",m_Values[i]->GetTypeName().c_str());
                }
            }
            
            m_ValueNames[i]->SetTextSize(m_TextSize);
            m_ValueNames[i]->RebuildMesh();
            if(m_CenterValueNames)
            {
                m_ValueNames[i]->SetPosition(Vec3((npSize.x * 0.5f) - (m_ValueNames[i]->GetBounds()->GetDimensions().x * 0.5f),
                                                  (npSize.y * 0.5f) - (m_ValueNames[i]->GetBounds()->GetDimensions().y * 0.5f),0.0f));
            }
            else
            {
                m_ValueNames[i]->SetPosition(Vec3(m_BorderSize,
                                                  (npSize.y * 0.5f) - (m_ValueNames[i]->GetBounds()->GetDimensions().y * 0.5f),0.0f));
            }
            
                                              
            m_ValueDisps[i]->SetText(Str);  
            m_ValueDisps[i]->SetTextSize(m_TextSize);
            m_ValueDisps[i]->RebuildMesh();
            
            if(m_CenterValues)
            {
                m_ValueDisps[i]->SetPosition(Vec3((vpSize.x * 0.5f) - (m_ValueDisps[i]->GetBounds()->GetDimensions().x * 0.5f),
                                                  (vpSize.y * 0.5f) - (m_ValueDisps[i]->GetBounds()->GetDimensions().y * 0.5f),0.0f));
            }
            else
            {
                m_ValueDisps[i]->SetPosition(Vec3(m_BorderSize,
                                                  (npSize.y * 0.5f) - (m_ValueDisps[i]->GetBounds()->GetDimensions().y * 0.5f),0.0f));
            }
        }
    }
};
