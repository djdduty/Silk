#pragma once
#include <UI/UIElement.h>
#include <Renderer/RenderObject.h>

namespace Silk
{
    class Camera;
    class Shader;
    class Renderer;
    class RenderUnformSet;
    
    class UIManager
    {
        public:
            UIManager(Renderer* r);
            ~UIManager();
        
            void Initialize();
        
            void SetViewScale(Vec2 Sc) { m_ViewScale = Sc; m_ViewNeedsUpdate = true; }
        
            void Render(PRIMITIVE_TYPE PrimType);
        
            UIElement* CreateElement();
            Shader * GetDefaultShader       () const { return m_DefaultShader       ; }
            Shader * GetDefaultTextureShader() const { return m_DefaultTextureShader; }
            Shader * GetDefaultTextShader   () const { return m_DefaultTextShader   ; }
        
            Camera* GetCamera() const { return m_Camera; }
        
        protected:
            friend class UIElement;
            
            Renderer* m_Renderer;
        
            Vec2 m_ViewScale;
            Vec2 m_Resolution;
            bool m_ViewNeedsUpdate;
            Camera  * m_Camera;
            Texture * m_View;
            Shader* m_DefaultShader;
            Shader* m_DefaultTextureShader;
            Shader* m_DefaultTextShader;
        
            RenderUnformSet* m_RenderUniforms;
        
            vector<UIElement*> m_Elements;
    };
};