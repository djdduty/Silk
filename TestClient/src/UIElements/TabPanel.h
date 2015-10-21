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
			UIPanel* GetTabView(i32 ID) const { if(ID < 0 || ID >= m_TabViews.size()) { return 0; } return m_TabViews[ID]; }
            i32 GetSelectedTabIndex() const { return m_SelectedTabIdx; }
        
        protected:
            void UpdatePanels();
            Scalar m_BorderSize;
            Scalar m_TextSize;
            i32 m_SelectedTabIdx;
        
            vector<UIPanel*> m_Tabs;
            vector<UIText *> m_TabLabels;
            vector<UIPanel*> m_TabViews;
        
            InputManager* m_Input;
    };
};