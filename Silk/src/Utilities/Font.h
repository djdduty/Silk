#pragma once
#include <Math/Math.h>

#include <vector>
using namespace std;

namespace Silk
{
    class Font
    {
        public:
            struct Glyph
            {
                Vec2 uv0;
                Vec2 uv1;
                Vec2 Size;
                Vec2 Offset;
                i32 xAdvance;
                
                bool IsValid;
            };
        
            Font();
            ~Font() { }
        
            void Load(Byte* Data);
        
            i32 GetGlyphCount() const { return m_GlyphCount; }
            Glyph GetGlyph(i32 CID) const { return m_Glyphs[CID]; }
        
            i32 GetGlyphSize() const { return m_GlyphSize; }
        
        protected:
            Glyph m_Glyphs[256];
            i32 m_GlyphCount;
            i32 m_GlyphSize; //In pixels
    };
};

