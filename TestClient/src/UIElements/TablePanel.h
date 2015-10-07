/*
 *  TablePanel.h
 *  TestClient
 *
 *  Created by Michael DeCicco on 10/5/15.
 *
 */

#pragma once
#include <Silk.h>
using namespace Silk;

namespace TestClient
{
    class InputManager;
    class TablePanel : public UIPanel
    {
        public:
            TablePanel(UIManager* Mgr,const Vec2& Size,InputManager* InputMgr);
            ~TablePanel();
        
            void OnMouseDown();
            void OnMouseMove();
            void OnMouseUp  ();
        
            void Clear();
        
            void SetBorderSize(Scalar Sz) { m_BorderSize = Sz; UpdatePanels(); }
            void SetTextSize  (Scalar Sz) { m_TextSize   = Sz; UpdatePanels(); }
            void SetMinSeparatorFactor(Scalar Fac);
            void SetMaxSeparatorFactor(Scalar Fac);
            void SetCenterValueNames(bool Flag);
            void SetCenterValues(bool Flag);
        
            i32 AddValue(const string& Name,ConfigValue* Val);
            ConfigValue* GetValue(const string& Name);
        
        protected:
            void UpdatePanels();
            bool m_SeparatorClicked;
            
            Scalar m_BorderSize;
            Scalar m_TextSize;
            Scalar m_SepFac;
            Scalar m_MinSepFac;
            Scalar m_MaxSepFac;
            Scalar m_TableValueBorderSize;
            bool m_CenterValueNames;
            bool m_CenterValues;
        
            UIPanel* m_NamePanel;
            UIText * m_Name;
            UIPanel* m_ValuePanel;
            UIText * m_Value;
            UIPanel* m_Separator;
        
            vector<UIPanel*> m_ValueNamePanels;
            vector<UIPanel*> m_ValueDispPanels;
            vector<UIText *> m_ValueNames;
            vector<UIText *> m_ValueDisps;
            vector<ConfigValue*> m_Values;
        
            InputManager* m_Input;
    };
};