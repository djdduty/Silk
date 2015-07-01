#include <LightingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>

namespace TestClient
{
    static string FragmentShaderPointLight =
    string("[PointLight]") +
    "\t\t\t\tvec3 Normal = normalize(o_Normal);\n" +
    "\t\t\t\tvec4 Color = texture(u_DiffuseMap,o_TexCoord);\n" +
    "\t\t\t\tvec3 Dir = u_Lights[l].Position.xyz - " + PositionOutName + ";\n" +
    "\t\t\t\tfloat Dist = length(Dir);\n" +
    "\t\t\t\tfloat FadeFactor = 1.0;\n" +
    "\t\t\t\tfloat MaxDist = 1.0 / sqrt(u_Lights[l].QAtt * 0.001);\n" +
    "\t\t\t\tif(Dist > MaxDist) discard;\n" +
    "\t\t\t\telse FadeFactor = 1.0 - ((Dist / MaxDist) * 2.0);\n" +
    "\t\t\t\tDir *= (1.0 / Dist);\n\n" +
    "\t\t\t\t//Compute specular power\n" +
    "\t\t\t\tfloat ndotl = max(dot(Normal,Dir),0.0);\n" +
    "\t\t\t\tfloat SpecularPower = 0.0;\n" +
    "\t\t\t\tif(ndotl > 0.0)\n" +
    "\t\t\t\t{\n" +
    "\t\t\t\t\tvec3 PosToCam       = normalize(u_CameraPosition - " + PositionOutName + ");\n" +
    "\t\t\t\t\tvec3 HalfDir        = normalize(Dir + PosToCam);\n" +
    "\t\t\t\t\tfloat SpecularAngle = max(dot(HalfDir,Normal),0.0);\n" +
    "\t\t\t\t\tSpecularPower       = pow(SpecularAngle,16.0f);\n" +
    "\t\t\t\t}\n\n" +
    "\t\t\t\t//Light equation\n" +
    "\t\t\t\tvec4 FinalColor = (0.2f * Color) + (ndotl * Color * u_Lights[l].Color) + (SpecularPower * u_Lights[l].Color * 1.0);\n" +
    "\t\t\t\t//Attenuation\n" +
    "\t\t\t\tFinalColor *= 1.0 / (u_Lights[l].CAtt + (u_Lights[l].LAtt * Dist) + (u_Lights[l].QAtt * (Dist * Dist)));\n" +
    "\t\t\t\tFinalColor *= u_Lights[l].Power * FadeFactor;\n" +
    "\t\t\t\t" + FragmentColorOutputName + " = FinalColor;\n" +
    "[/PointLight]";
    
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
        LoadLight();
    }
    void LightingTest::LoadLight()
    {
        m_Light0 = m_Renderer->CreateRenderObject(ROT_LIGHT, false);
        Light* L = new Light(LT_POINT);
        m_Light0->SetLight(L);
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 5;//?
        L->m_Attenuation.Constant    = 0.01f; //?
        L->m_Attenuation.Linear      = 0.01f; //?
        L->m_Attenuation.Exponential = 0.80f; //?
        m_Light0->SetTransform(Translation(Vec3(1,1,1)));
        m_Renderer->AddRenderObject(m_Light0);
        
        m_Light1 = m_Renderer->CreateRenderObject(ROT_LIGHT, false);
        L = new Light(LT_POINT);
        m_Light1->SetLight(L);
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 5;//?
        L->m_Attenuation.Constant    = 0.01f; //?
        L->m_Attenuation.Linear      = 0.01f; //?
        L->m_Attenuation.Exponential = 0.80f; //?
        m_Light1->SetTransform(Translation(Vec3(1,4,0)));
        m_Renderer->AddRenderObject(m_Light1);
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
        
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION         ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL           ,true);
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD         ,true);
        
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION         ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL           ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD         ,true);
        
        m_ShaderGenerator->SetTextureInput   (Material::MT_DIFFUSE                  ,true);
        m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR            ,true);
        m_ShaderGenerator->SetLightingMode(ShaderGenerator::LM_PHONG);
        
        m_ShaderGenerator->AddFragmentModule(const_cast<CString>(FragmentShaderPointLight.c_str()       ),0);
        m_ShaderGenerator->AddFragmentModule(const_cast<CString>("[SpotLight][/SpotLight]"              ),1);
        m_ShaderGenerator->AddFragmentModule(const_cast<CString>("[DirectionalLight][/DirectionalLight]"),2);
        
        /*
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[LtoS]vec3 LtoS  = o_Position - u_Lights[0].Position.xyz;[/LtoS]"                                             ),0);
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[Dist]float Dist = length(LtoS);\n\tLtoS *= (1.0 / Dist);\n[/Dist]"                                           ),1);
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[NdotL]float NdotL = dot(o_Normal,vec3(0,1,0));\n[/NdotL]"                                                    ),2);
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[Atten]float Att = u_Lights[0].CAtt + (u_Lights[0].LAtt * Dist) + (u_Lights[0].QAtt * Dist * Dist);\n[/Atten]"),3);
        m_ShaderGenerator->AddFragmentModule (const_cast<CString>("[SetColor]f_Color = texture(u_DiffuseMap,o_TexCoord) * Att;// * max(NdotL,0.2);[/SetColor]"                   ),4);
        */
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
                
                m_Diffuse->SetPixel(Vec2(x,(h-1) - y),C);
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
            //m_Object->SetTransform(Rotation(Vec3(0,1,0),a));
            
            Mat4 r = Rotation(Vec3(0,1,0),a * 6.0f);
            m_Light1->SetTransform(r * Translation(Vec3(2,4,0)));
            
            m_Renderer->Render(GL_TRIANGLES);
        }
    }

    void LightingTest::Shutdown()
    {
        delete m_Light0->GetLight();
        delete m_Light1->GetLight();
        m_Renderer->Destroy(m_Light0);
        m_Renderer->Destroy(m_Light1);
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
