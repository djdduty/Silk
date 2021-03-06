#include <ParallaxMappingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>

namespace TestClient
{
    ParallaxMappingTest::ParallaxMappingTest()
    {
    }
    ParallaxMappingTest::~ParallaxMappingTest()
    {
    }

    void ParallaxMappingTest::Initialize()
    {
        m_ObjLoader = new ObjLoader();
        LoadMaterial();
        LoadLight   ();
        LoadMesh    ();
    }
    void ParallaxMappingTest::LoadLight()
    {
        RenderObject* LObj = 0;
        Light* L = 0;
        
        LObj = m_Renderer->CreateRenderObject(ROT_LIGHT);
        L = new Light(LT_POINT);
        
        LObj->SetLight(L);
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;
        LObj->SetTransform(Translation(Vec3(0,1,-5)));
        
        m_Renderer->GetScene()->AddRenderObject(LObj);
        m_Lights.push_back(LObj);
        
        LObj = m_Renderer->CreateRenderObject(ROT_LIGHT);
        L = new Light(LT_SPOT);
        
        LObj->SetLight(L);
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 0.24;
        L->m_Cutoff = 45.0f;
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 0.10f;
        L->m_Attenuation.Exponential = 0.01f;
        LObj->SetTransform(Translation(Vec3(0,8,0)));
        
        m_Renderer->GetScene()->AddRenderObject(LObj);
        m_Lights.push_back(LObj);
        
        
        LObj = m_Renderer->CreateRenderObject(ROT_LIGHT);
        L = new Light(LT_DIRECTIONAL);
        
        LObj->SetLight(L);
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 0.1;
        
        m_Renderer->GetScene()->AddRenderObject(LObj);
        m_Lights.push_back(LObj);
    }
    void ParallaxMappingTest::LoadMesh()
    {
        m_ObjLoader->Load(const_cast<CString>("ParallaxMappingTest/Scene.object"));
        m_ObjLoader->ComputeTangents();
        
        m_Mesh = new Mesh();
        m_Mesh->SetVertexBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetPositions()));
        m_Mesh->SetNormalBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetNormals  ()));
        m_Mesh->SetTangentBuffer (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTangents ()));
        m_Mesh->SetTexCoordBuffer(m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTexCoords()));
        
        m_Object = m_Renderer->CreateRenderObject(ROT_MESH);
        m_Object->SetMesh(m_Mesh,m_Material);
        m_Renderer->GetScene()->AddRenderObject(m_Object);
        
        m_Object->SetTransform(Translation(Vec3(0,0,0)));
        m_Object->SetTextureTransform(Mat4::Identity);
        
        m_ObjLoader->Load(const_cast<CString>("ParallaxMappingTest/LightDisplay.object"));
        m_LDispMesh = new Mesh();
        m_LDispMesh->SetVertexBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetPositions()));
        m_LDispMesh->SetNormalBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetNormals  ()));
        m_LDispMesh->SetTexCoordBuffer(m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTexCoords()));
        
        for(i32 i = 0;i < m_Lights.size();i++)
        {
            RenderObject* d = m_Renderer->CreateRenderObject(ROT_MESH);
            d->SetMesh(m_LDispMesh,m_LDispMat);
            m_Renderer->GetScene()->AddRenderObject(d);
            m_LightDisplays.push_back(d);
        }
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void ParallaxMappingTest::LoadMaterial()
    {
        m_ShaderGenerator->SetShaderVersion  (330);
        m_ShaderGenerator->SetAllowInstancing(false);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION         ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL           ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD         ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TANGENT          ,true);
        
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION         ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL           ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD         ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT          ,true);
        
        m_ShaderGenerator->SetTextureInput   (Material::MT_DIFFUSE                  ,true);
        m_ShaderGenerator->SetTextureInput   (Material::MT_NORMAL                   ,true);
        m_ShaderGenerator->SetTextureInput   (Material::MT_PARALLAX                 ,true);
        m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR            ,true);
        m_ShaderGenerator->SetParallaxFunction(ShaderGenerator::PF_RELIEF);
        m_ShaderGenerator->SetLightingMode   (ShaderGenerator::LM_PHONG);
        
        //m_ShaderGenerator->AddFragmentModule(const_cast<CString>("[SetColor]vec4 sColor = vec4(sNormal,1);[/SetColor]"),0);
        
        m_Material = m_Renderer->CreateMaterial();
        m_Material->SetShininess(1.0f);
        m_Material->SetSpecular(1.0f);
        m_Material->SetShader(m_ShaderGenerator->Generate());
        
        m_Material->SetMinParallaxLayers(10);
        m_Material->SetMaxParallaxLayers(15);
        
        m_LDispMat = m_Renderer->CreateMaterial();
        m_LDispMat->SetShader(m_Material->GetShader());
        
        m_ShaderGenerator->Reset();
        
        vector<u8> Pixels;
        u32 w,h;
        f32 Inv255 = 1.0f / 255.0f;
        
        lodepng::decode(Pixels,w,h,"ParallaxMappingTest/Diffuse.png");
        m_Diffuse = m_Rasterizer->CreateTexture();
        m_Diffuse->CreateTexture(w,h);
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
                
                m_Diffuse->SetPixel(Vec2(x,(h-1) - y),C);
            }
        }
        m_Diffuse->UpdateTexture();
        
        Pixels.clear();
        lodepng::decode(Pixels,w,h,"ParallaxMappingTest/Normal.png");
        m_Normal = m_Rasterizer->CreateTexture();
        m_Normal->CreateTexture(w,h);
        for(i32 x = 0;x < w;x++)
        {
            for(i32 y = 0;y < h;y++)
            {
                i32 Idx = (y * (w * 4)) + (x * 4);
                
                /*
                 * For some reason, normal maps need to have X and Z flipped?
                 * Pay attention to this in the future, it will save a lot of
                 * trouble debugging things. I now know this from experience.
                 */
                
                Vec4 C;
                C.x = f32(Pixels[Idx + 2]) * Inv255; //Notice the + 2 (blue)
                C.y = f32(Pixels[Idx + 1]) * Inv255;
                C.z = f32(Pixels[Idx + 0]) * Inv255; //Notice the + 0 (red )
                C.w = f32(Pixels[Idx + 3]) * Inv255;
                
                m_Normal->SetPixel(Vec2(x,(h-1) - y),C);
            }
        }
        m_Normal->UpdateTexture();
        
        Pixels.clear();
        lodepng::decode(Pixels,w,h,"ParallaxMappingTest/Parallax.png");
        m_Parallax = m_Rasterizer->CreateTexture();
        m_Parallax->CreateTexture(w,h);
        for(i32 x = 0;x < w;x++)
        {
            for(i32 y = 0;y < h;y++)
            {
                i32 Idx = (y * (w * 4)) + (x * 4);
                
                Vec4 C;
                C.x = f32(Pixels[Idx + 2]) * Inv255;
                C.y = f32(Pixels[Idx + 1]) * Inv255;
                C.z = f32(Pixels[Idx + 0]) * Inv255;
                C.w = f32(Pixels[Idx + 3]) * Inv255;
                
                m_Parallax->SetPixel(Vec2(x,(h-1) - y),C);
            }
        }
        m_Parallax->UpdateTexture();
        
        m_Material->SetMap(Material::MT_DIFFUSE ,m_Diffuse );
        m_Material->SetMap(Material::MT_NORMAL  ,m_Normal  );
        m_Material->SetMap(Material::MT_PARALLAX,m_Parallax);
        
        m_LDispMat->SetMap(Material::MT_DIFFUSE ,m_Diffuse );
        m_LDispMat->SetMap(Material::MT_NORMAL  ,m_Normal  );
        m_LDispMat->SetMap(Material::MT_PARALLAX,m_Parallax);
    }

    void ParallaxMappingTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9));
        m_Camera->SetTransform(t);
        Scalar a = 0.0f;
        while(IsRunning())
        {
            a += 7.5f * GetDeltaTime();
            
            /*
            m_Camera->SetTransform((Rotation(Vec3(1,0,0),20 + sin(a * 0.300f) * 9.0f) *
                                    Rotation(Vec3(0,1,0),     sin(a * 0.250f) * 20.0f) *
                                    Rotation(Vec3(0,0,1),     sin(a * 0.125f) * 18.0f)) *
                                    Translation(Vec3(0,3,6)));
             */
            
            Mat4 r = Rotation(Vec3(0,1,0),a * 2.0f);
            
            m_Object->SetTransform(r);
            //m_Object->SetTextureTransform(Rotation(Vec3(0,0,1),(a * 1.2f) + (sin(a * 0.2f) * 10.0f)));
        
            m_Material->SetParallaxScale    (0.1f + (sin(a * 0.2) * 0.025f));
            
            //Point
            //m_Lights[0]->GetLight()->m_Attenuation.Exponential = 1.9f + (sin(a * 0.2f) * 0.5f);
            m_Lights[0]->GetLight()->m_Color = Vec4(ColorFunc(a * 0.01f),1.0f);
            m_Lights[0]->GetLight()->m_Power = 15.0f + (sin(a * 0.01f) * 5.0f);
            m_Lights[0]->SetTransform(Translation(Vec3(3,4 + (sin(a * 0.1f) * 3.0f),0)));
            
            //Spot
            m_Lights[1]->SetTransform(Translation(Vec3(0,6,0.0f)) * Rotation(Vec3(1,0,0),-90.0f) * Rotation(Vec3(0,1,0),180.0f + (sin(a * 0.075f) * 95.0f)));
            m_Lights[1]->GetLight()->m_Soften = (sin(a * 0.1f) * 0.5f) + 0.5f;
            
            //Directional
            m_Lights[2]->SetTransform(Rotation(Vec3(0,0,1),a * 2.0f) * Rotation(Vec3(1,0,0),-90.0f));
            
            for(i32 i = 0;i < m_Lights.size();i++)
            {
                //m_LightDisplays[i]->SetTransform(m_Lights[i]->GetTransform() * Scale(0.5f));
            }
        }
    }

    void ParallaxMappingTest::Shutdown()
    {
        for(i32 i = 0;i < m_Lights.size();i++)
        {
            delete m_Lights[i]->GetLight();
            m_Renderer->Destroy(m_Lights[i]);
            m_Renderer->Destroy(m_LightDisplays[i]);
        }
        m_Renderer->Destroy(m_Material);
        m_Renderer->Destroy(m_Object);
        m_Mesh->Destroy();
        delete m_ObjLoader;
        
        m_ObjLoader = 0;
        m_Material  = 0;
        m_Object    = 0;
        m_Mesh      = 0;
    }
};
