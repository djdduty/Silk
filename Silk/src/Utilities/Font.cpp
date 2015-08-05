#include <Utilities/Utilities.h>

namespace Silk
{
    Font::Font() : m_GlyphCount(0)
    {
        for(i32 i = 0;i < 256;i++) m_Glyphs[i].IsValid = false;
    }
    void Font::Load(Byte* Data)
    {
        for(i32 i = 0;i < 256;i++) m_Glyphs[i].IsValid = false;
        
        vector<string> Lines;
        string Buffer;
        while(*Data != 0)
        {
            char c = *Data;
            Data++;
            
            if(c == '\n')
            {
                Lines.push_back(Buffer);
                Buffer.clear();
            }
            else Buffer += c;
        }
        
        Vec2 CoordScale(1,1);
        
        for(i32 i = 0;i < Lines.size();i++)
        {
            size_t Idx = Lines[i].find("count");
            if(Idx != Lines[i].npos) { m_GlyphCount = ParseFirstInt(Lines[i]); continue; }
            
            Idx = Lines[i].find("id");
            if(Idx != Lines[i].npos)
            {
                //char id=97 x=2 y=2 width=21 height=24 xoffset=6 yoffset=10 xadvance=22 page=0 chnl=15
                vector<i32> Ints;
                if(ParseInts(Lines[i],Ints) >= 8)
                {
                    if(Ints[0] >= 256 || Ints[0] < 0)
                    {
                        WARNING("Invalid glyph ID (%d). Must be between 0 and 255.\n",Ints[0]);
                        continue;
                    }
                    
                    Glyph &g = m_Glyphs [ParseFirstIntAfter(Lines[i],"id")];
                    g.uv0 =         Vec2(ParseFirstIntAfter(Lines[i],"x"       ),ParseFirstIntAfter(Lines[i],"y"      ));
                    g.uv1 = g.uv0 + Vec2(ParseFirstIntAfter(Lines[i],"width"   ),ParseFirstIntAfter(Lines[i],"height" ));
                    g.Offset =      Vec2(ParseFirstIntAfter(Lines[i],"xoffset" ),ParseFirstIntAfter(Lines[i],"yoffset"));
                    g.xAdvance =         ParseFirstIntAfter(Lines[i],"xadvance");
                }
                else
                {
                    WARNING("Line:%d: Parsing \"char...\" line of font file, invalid formatting.\nExample: \"char id=97 x=2 y=2 width=21 height=24 xoffset=6 yoffset=10 xadvance=22 page=0 chnl=15\"\nErroneous line: %s\n",i,Lines[i].c_str());
                    continue;
                }
                continue;
            }
            
            Idx = Lines[i].find("common");
            if(Idx != Lines[i].npos)
            {
                //common lineHeight=64 base=46 scaleW=512 scaleH=512 pages=1 packed=0
                vector<i32> Ints;
                if(ParseInts(Lines[i],Ints) >= 2)
                {
                    CoordScale = Vec2(1.0f / Scalar(ParseFirstIntAfter(Lines[i],"scaleW")),1.0f / Scalar(ParseFirstIntAfter(Lines[i],"scaleH")));
                }
                else
                {
                    ERROR("Line:%d: Parsing \"common...\" line of font file, invalid formatting.\nExample: \"common lineHeight=64 base=46 scaleW=512 scaleH=512 pages=1 packed=0\"\nErroneous line: %s\n",i,Lines[i].c_str());
                    continue;
                }
                continue;
            }
            
            Idx = Lines[i].find("info");
            if(Idx != Lines[i].npos)
            {
                //info face=font size=39 bold=0 italic=0 charset= unicode= stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=0,0 outline=0
                i32 Sz = ParseFirstIntAfter(Lines[i],"size");
                if(Sz != INT32_MAX)
                {
                    m_GlyphSize = Sz;
                }
                else
                {
                    ERROR("Line:%d: Parsing \"info...\" line of font file, invalid formatting.\nExample: \"info face=font size=39 bold=0 italic=0 charset= unicode= stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=0,0 outline=0\"\nErroneous line: %s\n",i,Lines[i].c_str());
                    continue;
                }
            }
            
        }
        
        //Scale all glyph coordinates
        for(i32 i = 0;i < 256;i++)
        {
            m_Glyphs[i].uv0 *= CoordScale;
            m_Glyphs[i].uv1 *= CoordScale;
            m_Glyphs[i].Offset *= CoordScale;
        }
    }
};
