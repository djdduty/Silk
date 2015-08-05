#pragma once
#include <UI/UIElement.h>

namespace Silk
{
    class Renderer;
    class Camera;
    
    class UIManager
    {
        public:
            UIManager(Renderer* r);
            ~UIManager();
        
            void Initialize();
        
            void SetViewScale(Vec2 Sc) { m_ViewScale = Sc; m_ViewNeedsUpdate = true; }
        
            void Render();
        
            Camera* GetCamera() const { return m_Camera; }
        
        protected:
            friend class UIElement;
            UID NewUID() { m_NextUID++; return m_NextUID - 1; }
            UID m_NextUID;
        
            Renderer* m_Renderer;
            Camera* m_Camera;
        
            Vec2 m_ViewScale;
            Vec2 m_Resolution;
            bool m_ViewNeedsUpdate;
            Texture* m_View;
        
            vector<UIElement*> m_Elements;
    };
};