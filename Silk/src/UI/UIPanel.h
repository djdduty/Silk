#ifndef SILK_UI_PANEL_H
#define SILK_UI_PANEL_H

#include <UI/UIElement.h>

namespace Silk
{
    class UIPanel : public UIElement
    {
        public:
            UIPanel(Vec2 Size);
            ~UIPanel();

            void Update(Scalar dt);
            void OnInitialize();

            Vec4 GetBackgroundColor() { return m_BackgroundColor; }
            void SetBackgroundColor(Vec4 v) { m_BackgroundColor = v; if(m_Material) m_Material->SetDiffuse(v); }

        protected:
            void GenerateMesh();
            Vec4 m_BackgroundColor;
    };
};

#endif