#pragma once
#include <Silk.h>
using namespace Silk;

namespace TestClient
{
    class InputManager;
    class TabPanel : public UIPanel
    {
        public:
            TabPanel(const Vec2& Size,InputManager* InputMgr);
            ~TabPanel();
        
            void OnMouseDown();
        
            void SetBorderSize(Scalar Sz) { m_BorderSize = Sz; UpdatePanels(); }
            void SetTextSize  (Scalar Sz) { m_TextSize   = Sz; UpdatePanels(); }
        
            i32 AddTab(const string& TabName);
            UIPanel* GetTabView(i32 ID) const { return m_TabViews[ID]; }
        
        protected:
            void UpdatePanels();
            Scalar m_BorderSize;
            Scalar m_TextSize;
        
            vector<UIPanel*> m_Tabs;
            vector<UIText *> m_TabLabels;
            vector<UIPanel*> m_TabViews;
        
            InputManager* m_Input;
    };
};