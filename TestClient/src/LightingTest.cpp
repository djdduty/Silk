#include <LightingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>

namespace TestClient
{
    LightingTest::LightingTest()
    {
    }
    LightingTest::~LightingTest()
    {
    }

    void LightingTest::Initialize()
    {
        m_ObjLoader = new ObjLoader();
        LoadMaterial();
        LoadMesh();
    }
    void LightingTest::LoadMesh()
    {
        m_ObjLoader->Load(const_cast<CString>("LightingTest/Scene.object"));
        
        m_Mesh = new Mesh();
        m_Mesh->SetVertexBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetPositions()));
        m_Mesh->SetNormalBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetNormals  ()));
        m_Mesh->SetTexCoordBuffer(m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTexCoords()));
        
        m_Object = m_Renderer->CreateRenderObject(ROT_MESH,false);
        m_Object->SetMesh(m_Mesh,m_Material);
        m_Renderer->AddRenderObject(m_Object);
        
        m_Object->SetTransform(Translation(Vec3(0,0,0)));
        m_Object->SetTextureTransform(Mat4::Identity);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void LightingTest::LoadMaterial()
    {
        m_ShaderGenerator->SetShaderVersion  (330);
        m_ShaderGenerator->SetAllowInstancing(true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION       ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL         ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD       ,true);
        
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL        ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD      ,true);
        
        m_ShaderGenerator->SetTextureInput   (Material::MT_DIFFUSE                ,true);
        m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR          ,true);
        
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[NdotL]float NdotL = dot(o_Normal,vec3(0,1,0));\n[/NdotL]"),0);
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[SetColor]f_Color = texture(u_DiffuseMap,o_TexCoord);// * max(NdotL,0.2);[/SetColor]"),1);
        
        m_Material = m_Renderer->CreateMaterial();
        m_Material->SetShader(m_ShaderGenerator->Generate());
        m_ShaderGenerator->Reset();
        
        vector<u8> Pixels;
        u32 w,h;
        lodepng::decode(Pixels,w,h,"LightingTest/Diffuse.png");
        m_Diffuse = m_Rasterizer->CreateTexture();
        m_Diffuse->CreateTexture(w,h);
        f32 Inv255 = 1.0f / 255.0f;
        for(i32 x = 0;x < w;x++)
        {
            for(i32 y = 0;y < h;y++)
            {
                i32 Idx = (y * (w * 4)) + (x * 4);
                
                Vec4 C;
                C.x = f32(Pixels[Idx + 0]) * Inv255;
                C.y = f32(Pixels[Idx + 1]) * Inv255;
                C.z = f32(Pixels[Idx + 2]) * Inv255;
                C.w = f32(Pixels[Idx + 3]) * Inv255;
                
                m_Diffuse->SetPixel(Vec2(x,h - y),C);
            }
        }
        m_Diffuse->UpdateTexture();
        
        m_Material->SetMap(Material::MT_DIFFUSE,m_Diffuse);
    }

    void LightingTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,10));
        m_Camera->SetTransform(t);
        Scalar a = 0.0f;
        while(IsRunning())
        {
            a += 15.0f * GetDeltaTime();
            //m_Camera->SetTransform(t * Rotation(Vec3(0,1,0),a));
            m_Object->SetTransform(Rotation(Vec3(0,1,0),a));
            //m_Object->SetTextureTransform(Rotation(Vec3(0,0,1),a));
            
            m_Renderer->Render(GL_TRIANGLES);
        }
    }

    void LightingTest::Shutdown()
    {
        delete m_ObjLoader;
        m_Renderer->Destroy(m_Material);
        m_Renderer->Destroy(m_Object);
        m_Mesh->Destroy();
        
        m_ObjLoader = 0;
        m_Material  = 0;
        m_Object    = 0;
        m_Mesh      = 0;
    }
};
