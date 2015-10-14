#pragma once
#include <Silk.h>
using namespace Silk;

namespace TestClient
{
    class InputManager;
    class ScrollBar;
    
    class ScrollBarEventReceiver
    {
        public:
            ScrollBarEventReceiver() { }
            virtual ~ScrollBarEventReceiver() { }
        
            virtual void OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender) = 0;
    };
    
    class ScrollBar : public UIPanel
    {
        public:
            ScrollBar(const Vec2& Size,InputManager* InputMgr,UIElement* Parent);
            ~ScrollBar();
        
            virtual void OnMouseDown();
            virtual void OnMouseUp();
            virtual void OnMouseMove();
        
            void SetBorderSize(Scalar Sz) { m_BorderSize = Sz; UpdatePanels(); }
            void SetBarHeight (Scalar Ht) { m_BarHeight  = Ht; UpdatePanels(); }
            void SetReceiver  (ScrollBarEventReceiver* Receiver) { m_Receiver = Receiver; }
        
        protected:
            void UpdatePanels();
            Scalar m_BorderSize;
            Scalar m_BarHeight ;
            Scalar m_BarOffset ;
            Scalar m_BarFactor ;
            Scalar m_LastBarFac;
            bool   m_BarClicked;
        
            UIPanel* m_Bar;
        
            ScrollBarEventReceiver* m_Receiver;
            InputManager* m_Input;
    };
};