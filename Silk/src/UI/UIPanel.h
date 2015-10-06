#ifndef SILK_UI_PANEL_H
#define SILK_UI_PANEL_H

#include <UI/UIElement.h>
#include <Renderer/Texture.h>

namespace Silk
{
    class UIPanel : public UIElement
    {
        public:
            UIPanel(const Vec2& Size);
            ~UIPanel();

            void Update(Scalar dt);
            void OnInitialize();

            Vec4 GetBackgroundColor() { return m_BackgroundColor; }
            void SetBackgroundColor(const Vec4& v) { m_BackgroundColor = v; if(m_Material) m_Material->SetDiffuse(v); }
            void SetBackgroundImage(Texture* T) { m_BackgroundImage = T; if(m_Material) m_Material->SetMap(Material::MT_DIFFUSE,T); }

        protected:
            void GenerateMesh();
            Vec4 m_BackgroundColor;
            Texture* m_BackgroundImage;
    };
};

#endif