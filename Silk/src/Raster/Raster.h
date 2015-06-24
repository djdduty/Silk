#ifndef SILK_RASTERIZATION_H
#define SILK_RASTERIZATION_H

#include <Math/Math.h>
#include <Renderer/Material.h>
#include <Renderer/Light.h>
#include <Renderer/ShaderSystem.h>

#include <vector>
using namespace std;

namespace Silk
{
    class Mesh;
    class RasterObjectIdentifier
    {
        public:
            RasterObjectIdentifier() { }
            virtual ~RasterObjectIdentifier() { }
        
            virtual void SetMesh(Mesh* m) = 0;
            virtual void Render(i32 PrimitiveType,i32 Start,i32 Count) = 0;
    };
    
    class ColorFormat
    {
        public:
            enum COLOR_CHANNELS
            {
                CC_RGB,
                CC_RGBA,
                CC_GRAYSCALE,
            };
        
            ColorFormat(COLOR_CHANNELS c = CC_RGBA,u8 r = 8,u8 g = 8,u8 b = 8,u8 a = 8) :
                Channels(c), rbits(r), gbits(g), bbits(b), abits(a), graybits(0)
                { }
        
            ColorFormat(u8 GrayscaleBits) :
                Channels(CC_GRAYSCALE), rbits(0), gbits(0), bbits(0), abits(0), graybits(GrayscaleBits)
                { }
        
            void SetRGB (u8 r,u8 g,u8 b)      { Channels = CC_RGB      ; rbits = r; gbits = g; bbits = b; abits = 0; graybits = 0; }
            void SetRGBA(u8 r,u8 g,u8 b,u8 a) { Channels = CC_RGBA     ; rbits = r; gbits = g; bbits = b; abits = a; graybits = 0; }
            void SetGrayscale(u8 g,u8 a = 0)  { Channels = CC_GRAYSCALE; rbits = 0; gbits = 0; bbits = 0; abits = a; graybits = g; }
            
            COLOR_CHANNELS Channels;
            
            u8 rbits;
            u8 gbits;
            u8 bbits;
            u8 abits;
            u8 graybits;
    };
    
    class Rasterizer;
    class RasterContext
    {
        public:
            RasterContext(Rasterizer* Parent) : m_Parent(Parent) { }
            virtual ~RasterContext() { }
            ColorFormat m_Format;
            void SetResolution(const Vec2& Resolution);
            Vec2 GetResolution() const { return m_Resolution; }
        
        protected:
            Rasterizer* m_Parent;
            Vec2 m_Resolution;
            
    };
    
    class UniformCall;
    class Shader;
    
    enum UNIFORM_TYPE
    {
        UT_BOOL,
        UT_INT,
        UT_UINT,
        UT_FLOAT,
        UT_DOUBLE,
        UT_VEC2,
        UT_VEC3,
        UT_VEC4,
        UT_MAT4,
        UT_LIGHT,
    };
    class UniformDef
    {
        public:
            UniformDef() { }
            ~UniformDef();
            string Name;
            UNIFORM_TYPE Type;
            i32 Size;
            i32 PaddedSize;
            i32 TypeSize;
            i32 Offset;
            i32 ArraySize;
            vector<UniformCall*> PassCalls;
    };
    class UniformBuffer
    {
        public:
            virtual ~UniformBuffer() { }
        
            void ClearData();
        
            i32 DefineUniform(string Name);
            void SetUniform(i32 UID,const bool& Value);
            void SetUniform(i32 UID,const i32 &  Value);
            void SetUniform(i32 UID,const u32 &  Value);
            void SetUniform(i32 UID,const f32 &  Value);
            void SetUniform(i32 UID,const f64 &  Value);
            void SetUniform(i32 UID,const Vec2&  Value);
            void SetUniform(i32 UID,const Vec3&  Value);
            void SetUniform(i32 UID,const Vec4&  Value);
            void SetUniform(i32 UID,const Mat4&  Value);
            void SetUniform(i32 UID,const vector<bool>& Values);
            void SetUniform(i32 UID,const vector<i32 >& Values);
            void SetUniform(i32 UID,const vector<u32 >& Values);
            void SetUniform(i32 UID,const vector<f32 >& Values);
            void SetUniform(i32 UID,const vector<f64 >& Values);
            void SetUniform(i32 UID,const vector<Vec2>& Values);
            void SetUniform(i32 UID,const vector<Vec3>& Values);
            void SetUniform(i32 UID,const vector<Vec4>& Values);
            void SetUniform(i32 UID,const vector<Mat4>& Values);
            void SetUniform(i32 UID,Light* Lt);
            void SetUniform(i32 UID,const vector<Light*>& Lts);
        
            virtual void InitializeBuffer() = 0;
            virtual void UpdateBuffer    () = 0;
        
            i32 GetUniformCount() const { return m_UniformBuffer.size(); }
            UniformDef* GetUniformInfo(i32 Index) { return &m_UniformInfo [Index]; }
            void*    GetUniformPointer(i32 Index) { return m_UniformBuffer[Index]; }
            i32 GetBufferSize() const { return m_TotalSize; }
            i32 GetPaddedBufferSize() const { return m_TotalPaddedSize; }
            i32 GetUniformOffset(i32 UID) const;
            i32 GetPaddedUniformOffset(i32 UID) const;
        
            void SetUniformBlockInfo(const string& Name,ShaderGenerator::INPUT_UNIFORM_TYPE ID) { m_BlockName = Name; m_Type = ID; }
            string GetUniformBlockName() const { return m_BlockName; }
            ShaderGenerator::INPUT_UNIFORM_TYPE GetUniformBlockID() const { return m_Type; }
        
        protected:
            friend class Shader;
            friend class Rasterizer;
            UniformBuffer() : m_BlockName(""), m_Type(ShaderGenerator::IUT_COUNT), m_TotalSize(0), m_TotalPaddedSize(0), m_ParentIndex(-1), m_Parent(0) { }
            string m_BlockName;
            ShaderGenerator::INPUT_UNIFORM_TYPE m_Type;
            vector<void*> m_UniformBuffer;
            vector<UniformDef> m_UniformInfo;
            i32 m_TotalSize;
            i32 m_TotalPaddedSize;
            i32 m_ParentIndex;
            Shader* m_Parent;
    };
    
    class Shader
    {
        public:
            Shader(Renderer* r);
            virtual ~Shader() {}
        
            virtual i32 Load(CString VertexCode,CString GeometryCode,CString FragmentCode) = 0;
        
            bool UsesUniformInput(ShaderGenerator::INPUT_UNIFORM_TYPE Type) const { return m_UniformInputs[Type]; }
        
            virtual void Enable() = 0;
            virtual void PassUniforms(UniformBuffer* Uniforms) = 0;
            virtual void UseMaterial(Material* Mat) = 0;
            virtual void Disable() = 0;
        
        protected:
            friend class ShaderGenerator;
            bool m_UniformInputs[ShaderGenerator::IUT_COUNT];
            bool m_FragmentOutputs[ShaderGenerator::OFT_COUNT];
            Renderer* m_Renderer;
            i32 m_ID;
    };
    
    class Rasterizer
    {
        public:
            Rasterizer();
            virtual ~Rasterizer();
        
            void SetRenderer(Renderer* r) { m_Renderer = r; }
        
            virtual bool ValidateContext(RasterContext* Ctx);
            bool SetContext(RasterContext* Ctx);
            RasterContext* GetContext() const { return m_GraphicsContext; }
            virtual void InitializeContext() = 0;
        
            UniformBuffer* CreateUniformBuffer(ShaderGenerator::INPUT_UNIFORM_TYPE Type);
            Shader       * CreateShader ();
            Texture      * CreateTexture();
            void Destroy(UniformBuffer* Buffer);
            void Destroy(Shader       * S     );
            void Destroy(Texture      * T     );
        
            void SetClearColor(const Vec4& c) { m_ClearColor = c; }
        
            virtual void ClearActiveFramebuffer() = 0;
            virtual void SetViewport(i32 x,i32 y,i32 w,i32 h) = 0;

            virtual RasterObjectIdentifier* CreateObjectIdentifier() = 0;
        
            virtual RasterContext* CreateContext() = 0;
        protected:
            Renderer* m_Renderer;
            RasterContext* m_GraphicsContext;
            Vec4 m_ClearColor;
    };
}

#endif