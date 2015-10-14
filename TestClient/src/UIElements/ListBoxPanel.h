#pragma once
#include <Silk.h>
#include <UIElements/ScrollBar.h>
using namespace Silk;

namespace TestClient
{
    class InputManager;
    class ListBoxPanel;
    class ListBoxEventReceiver
    {
        public:
            ListBoxEventReceiver() { }
            virtual ~ListBoxEventReceiver() { }
        
            virtual void OnListSelectionChanged(i32 SelectionIndex,string SelectionName,ListBoxPanel* Sender) = 0;
    };
    
    class ListBoxPanel : public UIPanel, ScrollBarEventReceiver
    {
        public:
            enum TEXT_ALIGNMENT
            {
                TA_LEFT,
                TA_CENTER,
                TA_RIGHT
            };
        
            ListBoxPanel(const Vec2& Size,InputManager* InputMgr,UIElement* Parent);
            ~ListBoxPanel();
        
            void OnMouseDown();
            virtual void OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender);
        
            void SetTextAlign    (TEXT_ALIGNMENT Alignment);
            void SetBorderSize   (Scalar Sz) { m_BorderSize    = Sz; UpdatePanels(); }
            void SetTextSize     (Scalar Sz) { m_TextSize      = Sz; UpdatePanels(); }
            void SetScrollBarSize(Scalar Sz) { m_ScrollBarSize = Sz; UpdatePanels(); }
        
            void SetReceiver(ListBoxEventReceiver* Receiver) { m_Receiver = Receiver; }
        
            i32 AddEntry(const string& TabName);
        
        protected:
            void UpdatePanels();
            TEXT_ALIGNMENT m_TextAlignment;
            Scalar m_BorderSize;
            Scalar m_TextSize;
            Scalar m_ScrollBarSize;
            i32 m_SelectionIndex;
        
            ScrollBar* m_Scroll;
            Scalar m_ScrollOffset;
            vector<UIPanel*> m_Entries;
            vector<UIText *> m_EntryLabels;
        
            ListBoxEventReceiver* m_Receiver;
        
            InputManager* m_Input;
    };
};