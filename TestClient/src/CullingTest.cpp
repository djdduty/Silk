#include <CullingTest.h>
#include <LodePNG.h>
#include <ObjLoader.h>
#include <math.h>
#include <Utilities/SimplexNoise.h>

#include <System/TaskManager.h>

using namespace TestClient;
namespace TestClient
{
    CullingTest::CullingTest()
    {
    }
    CullingTest::~CullingTest()
    {
    }

    void CullingTest::Initialize()
    {
        m_ShaderGenerator->SetShaderVersion  (330);
        m_ShaderGenerator->SetAllowInstancing(false);
        
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput   (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
        m_ShaderGenerator->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
        m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_POSITION,true);
        
        m_ShaderGenerator->SetParallaxFunction(ShaderGenerator::PF_RELIEF);
        
        m_ObjLoader = new ObjLoader();
        LoadMaterial();
        LoadLights  ();
        LoadMeshes  ();
    }
    void CullingTest::LoadLights()
    {
        Light* L = 0;
        
        L = AddLight(LT_POINT,Vec3(0,1,-5))->GetLight();
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;

        L = AddLight(LT_SPOT,Vec3(0,8,0))->GetLight();
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 0.24;
        L->m_Cutoff = 45.0f;
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 0.10f;
        L->m_Attenuation.Exponential = 0.01f;
        
        L = AddLight(LT_DIRECTIONAL,Vec3(0,10,0))->GetLight();
        L->m_Color = Vec4(0.9,0.8,0.6,1);
        L->m_Power = 0.5f;
    }
    void CullingTest::LoadMeshes()
    {
        AddMesh("CullingTest/Scene.object",m_Materials[0],Vec3(0,0,0));
        i32 mid = AddMesh("CullingTest/LightDisplay.object",m_Materials[1],Vec3(0,0,0),m_Lights.size());
        for(i32 i = mid;i <= m_Lights.size();i++) m_LightMeshes.push_back(m_Meshes[i]);

		mid = AddMesh("CullingTest/CullObject.object",m_Materials[0],Vec3(0,2,0),NUM_OF_CULL_OBJECTS);
		for(i32 i = 0;i < NUM_OF_CULL_OBJECTS;i++)
		{
			m_Meshes[mid + i]->SetTransform(Translation(Vec3(Random(-100,100),Random(1,10),Random(-100,100))));
		}
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    void CullingTest::LoadMaterial()
    {
        Material* Mat = AddMaterial(ShaderGenerator::LM_PHONG,"CullingTest/GroundDiffuse.png",
                                                              "CullingTest/GroundNormal.png" ,
                                                              "CullingTest/GroundHeight.png");
        Mat->SetShininess(1.0f);
        Mat->SetSpecular(Vec4(1,1,1,0));
        
        Mat->SetMinParallaxLayers(MIN_PARALLAX_LAYERS);
        Mat->SetMaxParallaxLayers(MAX_PARALLAX_LAYERS);
        Mat->SetParallaxScale(0.01f);
        
        AddMaterial(ShaderGenerator::LM_FLAT,"CullingTest/GroundDiffuse.png");
    }
    RenderObject* CullingTest::AddLight(LightType Type,const Vec3& Pos)
    {
        RenderObject* LObj = m_Renderer->CreateRenderObject(ROT_LIGHT,false);
        Light* L = new Light(Type);
        
        LObj->SetLight(L);
        L->m_Attenuation.Constant    = 0.00f;
        L->m_Attenuation.Linear      = 2.10f;
        L->m_Attenuation.Exponential = 5.90f;
        LObj->SetTransform(Translation(Pos));
        
        m_Renderer->GetScene()->AddRenderObject(LObj);
        m_Lights.push_back(LObj);
        return LObj;
    }
    i32 CullingTest::AddMesh(const char* Path,Material* Mat,const Vec3& Pos,i32 Count)
    {
        m_ObjLoader->Load(const_cast<CString>(Path));
        m_ObjLoader->ComputeTangents();
        
        Mesh* M = new Mesh();
        M->SetVertexBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetPositions()));
        M->SetNormalBuffer  (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetNormals  ()));
        M->SetTangentBuffer (m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTangents ()));
        M->SetTexCoordBuffer(m_ObjLoader->GetVertCount (),const_cast<f32*>(m_ObjLoader->GetTexCoords()));
        
        RenderObject* Obj = 0;
        i32 First = m_Meshes.size();
        for(i32 i = 0;i < Count;i++)
        {
            Obj = m_Renderer->CreateRenderObject(ROT_MESH,false);
            Obj->SetMesh(M,Mat);
            m_Renderer->GetScene()->AddRenderObject(Obj);
            
            Obj->SetTransform(Translation(Pos));
            Obj->SetTextureTransform(Mat4::Identity);
            m_Meshes.push_back(Obj);
        }
        if(Count > 1) return First;
        return m_Meshes.size() - 1;
    }
    Texture* CullingTest::LoadTexture(const char *Path)
    {
        if(!Path) return 0;
        vector<u8> Pixels;
        u32 w,h;
        static f32 Inv255 = 1.0f / 255.0f;
        
        lodepng::decode(Pixels,w,h,Path);
        if(Pixels.size() == 0) return 0;
        
        Texture* Tex = m_Rasterizer->CreateTexture();
        Tex->CreateTexture(w,h);
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
                
				swap(C.x,C.z);
                
                Tex->SetPixel(Vec2(x,(h-1) - y),C);
            }
        }
        Tex->UpdateTexture();
        return Tex;
    }
    Material* CullingTest::AddMaterial(ShaderGenerator::LIGHTING_MODES LightingMode,const char* Diffuse,const char* Normal,const char* Parallax)
    {
        Material* Mat = m_Renderer->CreateMaterial();
        Texture* D = LoadTexture(Diffuse );
        Texture* N = LoadTexture(Normal  );
        Texture* P = LoadTexture(Parallax);
        
        if(D) { Mat->SetMap(Material::MT_DIFFUSE ,D); D->Destroy(); }
        if(N) { Mat->SetMap(Material::MT_NORMAL  ,N); N->Destroy(); }
        if(P) { Mat->SetMap(Material::MT_PARALLAX,P); P->Destroy(); }
        
        if(LightingMode != ShaderGenerator::LM_FLAT)
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_NORMAL  ,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
        }
        if(Diffuse)
        {
            m_ShaderGenerator->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
        }
        if(Normal || Parallax)
        {
            m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TANGENT ,true);
            m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeInput(ShaderGenerator::IAT_NORMAL  ,true);
            
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TANGENT ,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
            m_ShaderGenerator->SetAttributeOutput(ShaderGenerator::IAT_NORMAL  ,true);
        }
        
        m_ShaderGenerator->SetTextureInput(Material::MT_DIFFUSE ,D != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_NORMAL  ,N != 0);
        m_ShaderGenerator->SetTextureInput(Material::MT_PARALLAX,P != 0);
        m_ShaderGenerator->SetLightingMode(LightingMode);

        Mat->SetShader(m_ShaderGenerator->Generate());
        
        Mat->SetSpecular(Vec4(0,0,0,0));
        Mat->SetShininess(0.0f);
        
        m_Materials.push_back(Mat);
        return Mat;
    }

    void CullingTest::Run()
    {
        Mat4 t = Translation(Vec3(0,4,9)) * RotationX(20.0f);
        m_Camera->SetTransform(t);
         
		m_Meshes[0]->SetTextureTransform(Scale(GROUND_TEXTURE_SCALE));
        
        Scalar a = 0.0f;
        TaskManager* tm = new TaskManager();
        tm->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        tm->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
    
        while(IsRunning())
        {
            tm->BeginFrame();
        
            a += 7.5f * GetDeltaTime();
            
            m_Camera->SetTransform(RotationY(a * 0.3f) * Translation(Vec3(0,1,6)) * RotationX(20.0f));
            
            Mat4 r = Rotation(Vec3(0,1,0),a * 2.0f);
            //m_Meshes[0]->SetTransform(RotationZ(a));
            
            //m_Materials[0]->SetParallaxScale    (0.1f + (sin(a * 0.2) * 0.025f));
            
            //Point
            //m_Lights[0]->GetLight()->m_Attenuation.Exponential = 1.9f + (sin(a * 0.2f) * 0.5f);
            m_Lights[0]->GetLight()->m_Color = Vec4(ColorFunc(a * 0.01f),1.0f);
            m_Lights[0]->GetLight()->m_Power = 15.0f + (sin(a * 0.01f) * 5.0f);
            m_Lights[0]->SetTransform(Translation(Vec3(3,4 + (sin(a * 0.1f) * 3.0f),0)));
            
            //Spot
            //m_Lights[1]->SetTransform(Translation(Vec3(0,3,0.0f)) * Rotation(Vec3(1,0,0),-90.0f) * Rotation(Vec3(0,1,0),180.0f + (sin(a * 0.075f) * 95.0f)));
            //m_Lights[1]->GetLight()->m_Soften = (sin(a * 0.1f) * 0.5f) + 0.5f;
            
            //Directional
            m_Lights[2]->SetTransform(Rotation(Vec3(0,0,1),90 + (a)) * Rotation(Vec3(1,0,0),-90.0f));
            
            for(i32 i = 0;i < m_Lights.size();i++)
            {
                m_LightMeshes[i]->SetTransform(m_Lights[i]->GetTransform() * Scale(0.5f));
            }
            
            m_Renderer->Render(GL_TRIANGLES);
            
            tm->EndFrame();
        }
        
        delete tm;
    }

    void CullingTest::Shutdown()
    {
        for(i32 i = 0;i < m_Lights.size();i++)
        {
            delete m_Lights[i]->GetLight();
            m_Renderer->Destroy(m_Lights[i]);
        }
        m_Lights.clear();
        m_LightMeshes.clear();
        
        for(i32 i = 0;i < m_Meshes.size();i++)
        {
            m_Renderer->Destroy(m_Meshes[i]);
        }
        m_Meshes.clear();
        
        for(i32 i = 0;i < m_Materials.size();i++)
        {
            if(m_Materials[i]->GetMap(Material::MT_DIFFUSE )) m_Materials[i]->GetMap(Material::MT_DIFFUSE )->Destroy();
            if(m_Materials[i]->GetMap(Material::MT_NORMAL  )) m_Materials[i]->GetMap(Material::MT_NORMAL  )->Destroy();
            if(m_Materials[i]->GetMap(Material::MT_PARALLAX)) m_Materials[i]->GetMap(Material::MT_PARALLAX)->Destroy();
            if(m_Materials[i]->GetShader()) m_Renderer->GetRasterizer()->Destroy(m_Materials[i]->GetShader());
            m_Renderer->Destroy(m_Materials[i]);
        }
        m_Materials.clear();
        
        delete m_ObjLoader;
        m_ObjLoader = 0;
    }
};
