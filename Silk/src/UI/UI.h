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
        
            void SetCursorPosition(const Vec2& p);
            Vec2 GetCursorPosition() const { return m_CursorPosition; }
            Vec2 GetCursorDelta   () const { return m_CursorPosition - m_LastCursorPosition; }
            Vec2 GetUnBoundedCursorPosition() const { return m_UnBoundedCursorPosition; }
            Vec2 GetUnBoundedCursorDelta   () const { return m_UnBoundedCursorPosition - m_LastUnBoundedCursorPosition; }
            void ResetCursorDelta ()       { m_LastCursorPosition = m_CursorPosition; m_LastUnBoundedCursorPosition = m_UnBoundedCursorPosition; }
        
            void SetViewScale(Vec2 Sc) { m_ViewScale = Sc; m_ViewNeedsUpdate = true; }
        
            void Update(Scalar dt);
            void Render(Scalar dt,PRIMITIVE_TYPE PrimType);
        
            void AddElement(UIElement* Element);
            void RemoveElement(UIElement* Element);
        
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
        
            Vec2 m_LastUnBoundedCursorPosition;
            Vec2 m_UnBoundedCursorPosition;
            Vec2 m_LastCursorPosition;
            Vec2 m_CursorPosition;
            Vec2 m_RealCursorPosition;
    };
};