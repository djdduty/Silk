/*
 *  ActorPanel.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 10/4/15.
 *
 */

#include <T4/ActorPanel.h>

#define PANEL_WIDTH 300
#define TEXT_HEIGHT 22
UIText* AddText(UIManager* Mgr,const Vec2& Pos,const string& Text,UIPanel* Panel)
{
    UIText* Tex = new UIText();
    Tex->SetFont(Mgr->GetFont());
    Tex->SetText(Text);
    Tex->SetTextSize(22);
    Tex->SetPosition(Vec3(Pos,0.0f));
    Panel->AddChild(Tex);
    return Tex;
}
ActorPanel::ActorPanel(UIManager* Mgr,InputManager* Input) : UIPanel(Vec2(0,0))
{
    SetSize(Vec2(PANEL_WIDTH,Mgr->GetResolution().y));
    SetPosition(Vec3(0,0,0));
    SetBackgroundColor(Vec4(0.325f,0.325f,0.325f,1.0f));
    Mgr->AddElement(this);
    m_Translation = 0.0;
    m_Velocity = -1.0f;
    
    m_Heading   = AddText(Mgr,Vec2(2,2),"Actor Data",this);
    
    m_TabPanel  = new TabPanel(Vec2(PANEL_WIDTH - 4.0f,Mgr->GetResolution().y - 6.0f - 22.0f),Input);
    m_TabPanel->SetPosition(Vec3(2.0f,4.0f + 22.0f,0.0f));
    Mgr->AddElement(m_TabPanel);
    AddChild(m_TabPanel);
    m_TabPanel->AddTab("Info");
    m_TabPanel->AddTab("Variables");
    
    m_VarTable = new TablePanel(m_Manager,m_TabPanel->GetTabView(1)->GetBounds()->GetDimensions() - Vec2(4.0f,4.0f),Input);
    m_TabPanel->GetTabView(1)->AddChild(m_VarTable);
    m_VarTable->SetMinSeparatorFactor(0.22f);
    m_VarTable->SetMaxSeparatorFactor(0.82f);
    m_VarTable->SetPosition(Vec3(2.0f,2.0f,0.0f));
    
    m_ActorName = AddText(Mgr,Vec2(2,2 + (TEXT_HEIGHT * 0.0f)),"Name: ",m_TabPanel->GetTabView(0));
    m_ActorFile = AddText(Mgr,Vec2(2,2 + (TEXT_HEIGHT * 2.0f)),"File: ",m_TabPanel->GetTabView(0));
    m_ActorID   = AddText(Mgr,Vec2(2,2 + (TEXT_HEIGHT * 4.0f)),"ID:   ",m_TabPanel->GetTabView(0));
}
ActorPanel::~ActorPanel()
{
}
void ActorPanel::Toggle()
{
    if(m_Translation == 1.0f || m_Velocity > 0.0f) m_Velocity = -4.5f;
    else if(m_Translation == 0.0f || m_Velocity < 0.0f) { m_Velocity = 4.5f; SetEnabled(true); }
}
void ActorPanel::Update(Scalar dt)
{
    if(m_Velocity != 0.0f)
    {
        m_Translation += m_Velocity * dt;
        
        if(m_Translation > 1.0f)
        {
            m_Translation = 1.0f;
            m_Velocity = 0.0f;
        }
        
        if(m_Translation < 0.0f)
        {
            m_Translation = 0.0f;
            m_Velocity = 0.0f;
            
            SetEnabled(false);
        }
        
        Scalar xOffset = -PANEL_WIDTH + (PANEL_WIDTH * m_Translation);
        SetPosition(Vec3(xOffset,GetPosition().y,0.0f));
        m_Manager->GetRenderer()->GetRasterizer()->SetViewport(xOffset + PANEL_WIDTH,GetPosition().y,m_Manager->GetResolution().x - (xOffset + PANEL_WIDTH),m_Manager->GetResolution().y - GetPosition().y);
    }
}

void ActorPanel::SetActor(Actor* a)
{
    if(a->GetDef())
    {
        m_ActorName->SetText(string("Name: \n") + a->GetDef()->Name);
        string fn = a->GetDef()->ActorFile;
        fn = fn.substr(fn.find_last_of("\\"),fn.length());
        m_ActorFile->SetText(string("File: \n") + fn);
        m_ActorID->SetText(FormatString("ID: %d",a->GetDef()->ID));
        
        m_VarTable->Clear();
        ActorVariables* v = a->GetActorVariables();
        if(v)
        {
            for(i32 i = 0;i < v->GetBlockCount();i++)
            {
                ByteStream* d = v->GetBlock(i)->GetData();
                d->SetOffset(0);
                ConfigValue* val = 0;
                
                if(d->GetSize() == 1)
                {
                    char c = d->GetByte();
                    val = new ConfigValue(ConfigValue::VT_BYTE,&c);
                }
                else if(d->GetSize() == 2)
                {
                    void* ptr = d->Ptr();
                    val = new ConfigValue(ConfigValue::VT_U16,ptr);
                }
                else if(d->GetSize() == 4)
                {
                    void* ptr = d->Ptr();
                    float    fv = *((f32*)ptr);
                    int      iv = *((i32*)ptr);
                    
                    if(fabs(fv) < 1000000.0f && (fabs(fv) > 0.0001f || fabs(fv) == 0.0f)) val = new ConfigValue(ConfigValue::VT_F32,ptr);
                    if(abs(iv) < 20000) val = new ConfigValue(ConfigValue::VT_I32,ptr);
                }
                else
                {
                    string str;
                    if(((Byte*)d->Ptr())[d->GetSize() - 1] != 0) for(i32 b = 0;b < d->GetSize();b++) str += FormatString("%02X ",(u8)d->GetByte());
                    else for(i32 b = 0;b < d->GetSize() - 1;b++) str += d->GetByte();
                    val = new ConfigValue(ConfigValue::VT_STRING,&str);
                }
                if(val) m_VarTable->AddValue(v->GetBlock(i)->GetTypeString(),val);
            }
        }
    }
    else m_ActorName->SetText("Static Mesh");
}