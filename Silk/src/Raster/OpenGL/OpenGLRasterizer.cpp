#include <Raster/OpenGL/OpenGLRasterizer.h>

namespace Silk
{
    OpenGLRasterizerContext::~OpenGLRasterizerContext()
    {
    }

    OpenGLRasterizer::OpenGLRasterizer()
    {
    }
    OpenGLRasterizer::~OpenGLRasterizer()
    {
    }
    
    #define FormatCheck4(r,g,b,a) (f.rbits == (r) && f.gbits == (g) && f.bbits == (b) && f.abits == (a))
    #define FormatCheck3(r,g,b)   FormatCheck4(r,g,b,f.abits)
    #define FormatCheckG(g,a)     (f.graybits == (g) && f.abits == (a))
    
    bool OpenGLRasterizer::ValidateContext(RasterContext *Ctx)
    {
        ColorFormat &f = Ctx->m_Format;
        switch(f.Channels)
        {
            case ColorFormat::CC_RGB:
            {
                if     (FormatCheck3(3 ,3 ,2    )) m_ColorFormat = GL_R3_G3_B2              ;
                else if(FormatCheck3(4 ,4 ,4    )) m_ColorFormat = GL_RGB4                  ;
                else if(FormatCheck3(5 ,5 ,5    )) m_ColorFormat = GL_RGB5                  ;
                else if(FormatCheck3(8 ,8 ,8    )) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck3(10,10,10   )) m_ColorFormat = GL_RGB10                 ;
                else if(FormatCheck3(12,12,12   )) m_ColorFormat = GL_RGB12                 ;
                else if(FormatCheck3(16,16,16   )) m_ColorFormat = GL_RGB16                 ;
                else
                {
                    ERROR("Incompatible color format CC_RGB (%d,%d,%d)\n",(i32)f.rbits,
                                                                          (i32)f.gbits,
                                                                          (i32)f.bbits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_RGBA:
            {
                if     (FormatCheck4(2 , 2, 2, 2)) m_ColorFormat = GL_RGBA2                 ;
                else if(FormatCheck4(4 , 4, 4, 4)) m_ColorFormat = GL_RGBA4                 ;
                else if(FormatCheck4(5 , 5, 5, 1)) m_ColorFormat = GL_RGB5_A1               ;
                else if(FormatCheck4(8 , 8, 8, 8)) m_ColorFormat = GL_RGB8                  ;
                else if(FormatCheck4(10,10,10, 2)) m_ColorFormat = GL_RGB10_A2              ;
                else if(FormatCheck4(12,12,12,12)) m_ColorFormat = GL_RGBA12                ;
                else if(FormatCheck4(16,16,16,16)) m_ColorFormat = GL_RGBA16                ;
                else
                {
                    ERROR("Incompatible color format CC_RGBA (%d,%d,%d,%d)\n",(i32)f.rbits,
                                                                              (i32)f.gbits,
                                                                              (i32)f.bbits,
                                                                              (i32)f.abits);
                    return false;
                }
                break;
            }
            case ColorFormat::CC_GRAYSCALE:
            {
                if     (FormatCheckG(4 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(8 ,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(12,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(16,0       )) m_ColorFormat = GL_LUMINANCE4            ;
                else if(FormatCheckG(4 ,4       )) m_ColorFormat = GL_LUMINANCE4_ALPHA4     ;
                else if(FormatCheckG(6 ,2       )) m_ColorFormat = GL_LUMINANCE6_ALPHA2     ;
                else if(FormatCheckG(8 ,8       )) m_ColorFormat = GL_LUMINANCE8_ALPHA8     ;
                else if(FormatCheckG(12,4       )) m_ColorFormat = GL_LUMINANCE12_ALPHA4    ;
                else if(FormatCheckG(12,12      )) m_ColorFormat = GL_LUMINANCE12_ALPHA12   ;
                else if(FormatCheckG(16,16      )) m_ColorFormat = GL_LUMINANCE16_ALPHA16   ;
                else
                {
                    if(f.abits == 0) ERROR("Incompatible color format CC_GRAYSCALE%d\n"        ,(i32)f.graybits);
                    else             ERROR("Incompatible color format CC_GRAYSCALE (%d,A:%d)\n",(i32)f.graybits,
                                                                                                (i32)f.abits);
                    return false;
                }
                break;
            }
            default:
            {
                ERROR("Incompatible color format: Invalid value for ColorFormat::Channels. (Memory corruption?)\n");
                return false;
            }
        }
        
        return true;
    }
    void OpenGLRasterizer::InitializeContext()
    {  
    }
};
