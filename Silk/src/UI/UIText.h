#pragma once
#include <Utilities/Font.h>
#include <UI/UIContent.h>
#include <UI/UI.h>

namespace Silk
{

    /* 
     * TO DO:
     * - Alignment options
     * - In-string character modifiers (like [i][/i])
     * - Proper vertical spacing (accounting for character scales)
     */
    
    class UIText : public UIRenderContent
    {
        public:
            struct CharacterMods
            {
                Vec4   Color ;
                Vec2   Scale ;
                Vec2   Offset;
                Scalar Angle ;
            };

            UIText(UIManager* Manager, UIElementStyle* Style);
            ~UIText();

            void UpdateMesh     ();
            void UpdateMaterial ();
            void UpdateTransform();
            void OnRender(PRIMITIVE_TYPE PrimType);
        
            void  SetFont(Font* Fnt);
            Font* GetFont() const { return m_Font; }

            void   SetScale(Scalar s);
            Scalar GetScale() const { return m_Scale; }
        
            void          SetCharacterModifiers(const vector<CharacterMods>& CharMods);
            void          SetCharacterModifiers(i32 Idx,const CharacterMods& Mods);
            CharacterMods GetCharacterModifiers(i32 Idx) { return m_CharMods[Idx]; }
        
            void SetCharacterColor (i32 Idx,const Vec4& Color );
            void SetCharacterScale (i32 Idx,const Vec2& Scale );
            void SetCharacterScale (i32 Idx,Scalar      Scale );
            void SetCharacterOffset(i32 Idx,const Vec2& Offset);
            void SetCharacterAngle (i32 Idx,Scalar      Angle );
        
            Vec4   GetCharacterColor (i32 Idx) const { return m_CharMods[Idx].Color ; }
            Vec2   GetCharacterScale (i32 Idx) const { return m_CharMods[Idx].Scale ; }
            Vec2   GetCharacterOffset(i32 Idx) const { return m_CharMods[Idx].Offset; }
            Scalar GetCharacterAngle (i32 Idx) const { return m_CharMods[Idx].Angle ; }
        
            void SetColor(const Vec4& Color);
            Vec4 GetColor() const { return m_Color; }
        
            void   SetText(const string& Text);
            string GetText() const { return m_Text; }
        
            void      SetMaterial(Material* Mat) { m_Material = Mat; }
            Material* GetMaterial() const { return m_Material; }

            Vec2 GetSize();
        
        protected:
            void RebuildMesh();
            void RebuildCharacter(i32 Idx);
        
            Vec4                  m_Color;
            Scalar                m_Scale;
            bool                  m_TextChanged;
            vector<CharacterMods> m_CharMods;
            string                m_Text;
            Font*                 m_Font;
            Material*             m_Material;
            Vec2                  m_BoundSize;
    };
};

