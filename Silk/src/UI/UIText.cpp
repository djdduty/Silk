/*
 *  UIText.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 8/10/15.
 *
 *  I like this.... this is mine now. - DJD 9/12/2015 ;)
 */

#include "UIText.h"

namespace Silk
{
    UIText::UIText() : m_Font(0), m_Color(Vec4(1,1,1,1)), m_Scale(1.0f), m_TextChanged(false)
    {
    }
    UIText::~UIText()
    {
    }
    void UIText::Update(Scalar dt) 
    { 
        if(m_TextChanged == true)
            RebuildMesh();
    }

    void UIText::SetFont(Font* Fnt)
    {
        m_Font = Fnt;
        if(m_Material)
            m_Material->SetMap(Material::MT_DIFFUSE,m_Font->GetFontImage());
        m_TextChanged = true;
    }
    void UIText::OnInitialize()
    {
        m_Material->SetShader(m_Manager->GetDefaultTextShader());
        if(m_Font)
            m_Material->SetMap(Material::MT_DIFFUSE, m_Font->GetFontImage());
    }

    void UIText::SetScale(Scalar s)
    {
        m_Scale = s;
        m_TextChanged = true;
    }

    void UIText::SetCharacterModifiers(const vector<CharacterMods>& CharMods)
    {
        if(CharMods.size() != m_Text.size())
        {
            WARNING("Character modifier array is not the same size as text.\n");
            return;
        }
        m_CharMods = CharMods;
        m_TextChanged = true;
    }
    void UIText::SetCharacterModifiers(i32 Idx,const CharacterMods& Mods)
    {
        m_CharMods[Idx] = Mods;
        m_TextChanged = true;
    }

    void UIText::SetCharacterColor (i32 Idx,const Vec4& Color )
    {
        m_CharMods[Idx].Color = Color;
        m_TextChanged = true;
    }
    void UIText::SetCharacterScale (i32 Idx,const Vec2& Scale )
    {
        m_CharMods[Idx].Scale = Scale;
        m_TextChanged = true;
    }
    void UIText::SetCharacterScale (i32 Idx,Scalar      Scale )
    {
        m_CharMods[Idx].Scale = Vec2(Scale,Scale);
        m_TextChanged = true;
    }
    void UIText::SetCharacterOffset(i32 Idx,const Vec2& Offset)
    {
        m_CharMods[Idx].Offset = Offset;
        m_TextChanged = true;
    }
    void UIText::SetCharacterAngle (i32 Idx,Scalar      Angle )
    {
        m_CharMods[Idx].Angle = Angle;
        m_TextChanged = true;
    }
    void UIText::SetColor(const Vec4 &Color)
    {
        m_Color = Color;
        for(i32 i = 0;i < m_CharMods.size();i++) m_CharMods[i].Color = Color;
        m_TextChanged = true;
    }
    void UIText::SetText(const string &Text)
    {
        m_Text = Text;
        m_TextChanged = true;
        
        if(m_Text.size() != m_CharMods.size())
        {
            i32 Diff = m_Text.size() - m_CharMods.size();
            if(Diff < 0) m_CharMods.erase(m_CharMods.end() + Diff,m_CharMods.end());
            else
            {
                for(i32 i = 0;i < Diff;i++)
                {
                    CharacterMods m;
                    m.Color  = m_Color;
                    m.Scale  = Vec2(1,1);
                    m.Offset = Vec2(0,0);
                    m.Angle  = 0.0f;
                    m_CharMods.push_back(m);
                }
            }
        }
    }

    void UIText::RebuildMesh()
    {
        Mesh* m = new Mesh();

        if(!m_Render)
            m_Render = m_Manager->GetRenderer()->CreateRenderObject(ROT_MESH);
        
        vector<Vec3> Verts ;
        vector<Vec2> UVs   ;
        vector<Vec4> Colors;
        
        Scalar xOffset = 0;
        Scalar yOffset = 0;
        f32 GlyphSize = m_Font->GetGlyphSize();
        Scalar zOffset = 0;//m_Render->GetTransform().GetTranslation().z;
        
        /*
         * vert layout
         *  0---1
         *  |   |
         *  |   |
         *  3___2
         */
        
        f32 biggestXOff = 0.0;
        f32 lastXSize = 0.0;
        for(i32 i = 0;i < m_Text.length();i++)
        {
            if(m_Text[i] == '\n')
            {
                yOffset += GlyphSize * m_Scale;
                if(xOffset > biggestXOff) biggestXOff = xOffset;
                xOffset  = 0.0f;
                continue;
            }
            if(m_Text[i] == '[')
            {
                /*
                 * Parse for modifiers:
                 * Color   : [C: r,g,b,a][/C]
                 * Scale   : [S: x,y][/S]
                 * Scale   : [S: s][/S]
                 * Offset  : [O: x,y][/O]
                 * Rotation: [R: a][/R]
                 */
            }
            Font::Glyph g = m_Font->GetGlyph(m_Text[i]);
            if(!g.IsValid) continue;
            
            Vec2 p0 ,p1 ,p2 ,p3 ;
            Vec2 uv0,uv1,uv2,uv3;
            Vec2 Offset = m_CharMods[i].Offset + (g.Offset * m_CharMods[i].Scale * m_Scale);
            
            p0 = Vec2(xOffset,yOffset);
            p2 = p0 + Vec2(g.Size.x * m_Scale,g.Size.y * m_Scale) * m_CharMods[i].Scale;
            p1 = p0 + ((p2 - p0) * Vec2(1,0));
            p3 = p0 + ((p2 - p0) * Vec2(0,1));
            
            uv0 = g.uv0;
            uv1 = uv0 + ((g.uv1 - g.uv0) * Vec2(1,0));
            uv2 = g.uv1;
            uv3 = uv0 + ((g.uv1 - g.uv0) * Vec2(0,1));
            
            Verts.push_back(Vec3(p0 + Offset,zOffset));
            Verts.push_back(Vec3(p1 + Offset,zOffset));
            Verts.push_back(Vec3(p2 + Offset,zOffset));
            
            Verts.push_back(Vec3(p0 + Offset,zOffset));
            Verts.push_back(Vec3(p2 + Offset,zOffset));
            Verts.push_back(Vec3(p3 + Offset,zOffset));
            
            UVs  .push_back(uv0);
            UVs  .push_back(uv1);
            UVs  .push_back(uv2);
            
            UVs  .push_back(uv0);
            UVs  .push_back(uv2);
            UVs  .push_back(uv3);
            
            Colors.push_back(m_CharMods[i].Color);
            Colors.push_back(m_CharMods[i].Color);
            Colors.push_back(m_CharMods[i].Color);
            
            Colors.push_back(m_CharMods[i].Color);
            Colors.push_back(m_CharMods[i].Color);
            Colors.push_back(m_CharMods[i].Color);
            
            xOffset += g.xAdvance * m_Scale * m_CharMods[i].Scale.x;
            lastXSize = g.xAdvance * m_Scale * m_CharMods[i].Scale.x;
        }
        Vec2 BoundSize = Vec2(0,0);
        BoundSize.y = yOffset + (GlyphSize * m_Scale);
        f32 totalXSize = xOffset + lastXSize;
        if(totalXSize > biggestXOff) biggestXOff = totalXSize;
        BoundSize.x = biggestXOff;
        m_Bounds->SetDimensions(BoundSize);
        
        m->SetVertexBuffer  (m_Text.length() * 6,&Verts [0].x);
        m->SetTexCoordBuffer(m_Text.length() * 6,&UVs   [0].x);
        m->SetColorBuffer   (m_Text.length() * 6,&Colors[0].x);
        
        m_Render->SetMesh(m,m_Material);
        m->Destroy();
        m_TextChanged = false;
    }
};
