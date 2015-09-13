#ifndef SILK_UI_CONTENT_H
#define SILK_UI_CONTENT_H

#include <Renderer/Renderer.h>

namespace Silk
{
    class RendorObject;
    class UIElementStyle;
    class UIManager;

    class UIRenderContent
    {
        public:
            UIRenderContent(UIManager* Manager, UIElementStyle* Style);
            virtual ~UIRenderContent();

            virtual void UpdateMesh     ()=0;
            virtual void UpdateMaterial ()=0;
            virtual void UpdateTransform()=0;
            RenderObject* GetRender()        { return m_Render; }
            void SetStyle(UIElementStyle* S) { m_Style = S;     } //TODO Make UIElementStyle a ref counter, call destroy here
            UIElementStyle* GetStyle()       { return m_Style;  }

            virtual void Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);
            virtual void OnRender(PRIMITIVE_TYPE PrimType) {}

        protected:
            RenderObject*   m_Render;
            UIManager*      m_Manager;
            UIElementStyle* m_Style;
    };

    class UIRenderRectangle : public UIRenderContent
    {
        public:
            UIRenderRectangle(UIManager* Manager, UIElementStyle* Style);

            void UpdateMesh     ();
            void UpdateMaterial ();
            void UpdateTransform();
    };
}

#endif