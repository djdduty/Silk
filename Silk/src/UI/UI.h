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
        
            void Update(Scalar dt);
            void Render(Scalar dt,PRIMITIVE_TYPE PrimType);
        
            void     AddElement(UIElement* Element);
            void     RemoveElement(UIElement* Element);
        
            Shader * GetDefaultShader       () const { return m_DefaultShader       ; }
            Shader * GetDefaultTextureShader() const { return m_DefaultTextureShader; }
            Shader * GetDefaultTextShader   () const { return m_DefaultTextShader   ; }
        
            Camera*   GetCamera  () const { return m_Camera; }
            Vec2      GetResolution() { return m_Resolution; }
            Renderer* GetRenderer() { return m_Renderer; }

            void SetTransform(Mat4 Trans);
            void SetZClipPlanes(f32 n, f32 f);

            void SetFont(Font* Fnt) { m_Font = Fnt; }
            Font* GetFont() { return m_Font; }

            void OnMouseMove();
            void OnMouseOver() {}
            void OnMouseDown();
            void OnMouseOut () {}
            void OnMouseUp  ();
            void OnKeyDown  () {}
            void OnKeyHeld  () {}
            void OnKeyUp    () {}

			void SetViewNeedsUpdate(bool needsUpdate) { m_ViewNeedsUpdate = needsUpdate; }
        
        protected:
            friend class UIElement;
            friend class UIRenderContent;

            Renderer* m_Renderer;
        
            Font* m_Font;
            Vec2  m_ViewScale;
            Vec2  m_Resolution;
            bool  m_ViewNeedsUpdate;
            Camera  * m_Camera;
            Texture * m_View;
            Shader* m_DefaultShader;
            Shader* m_DefaultTextureShader;
            Shader* m_DefaultTextShader;
            Mesh*   m_DefaultRectangleMesh;
        
            RenderUnformSet* m_RenderUniforms;
        
            vector<UIElement*> m_Elements;
    };
};