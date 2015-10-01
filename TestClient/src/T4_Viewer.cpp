/*
 *  T4_Viewer.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#include <T4_Viewer.h>
#include <T4/ATRLoader.h>

#include <T4/SOIL/SOIL.h>

#include <UI/UIText.h>

namespace TestClient
{
    T4_Viewer::T4_Viewer(i32 ArgC,char* ArgV[])
    {
        //Check for arguments
        if(ArgC < 3)
        {
			//-d currently does nothing
            cout << "Usage: T4_ATR <Turok base folder> <ATR file> (Optional -d <Output folder>)" << endl;
            m_DoShutdown = true;
            return;
        }
        
        LoadATR(ArgV[1],ArgV[2]);
    }
    T4_Viewer::~T4_Viewer()
    {
    }
    
    bool T4_Viewer::LoadATR(const string &TurokDir,const string &File)
    {
        if(!m_ATR.Load(TurokDir,File))
        {
            cout << "Unable to load ATR file <" << File << ">. " << m_ATR.GetErrorString() << "." << endl;
            return false;
        }
    }
    void T4_Viewer::AddActorToScene(Actor *a,bool IsStatic)
    {
        string fn = a->GetFilename();
        cout << "Adding actor to scene <" << fn << ">." << endl;
        ActorMesh* m = a->GetMesh();
        if(!m) return;
        
        m_Actors.push_back(SilkObjectVector());
        for(i32 s = 0;s < m->GetSubMeshCount();s++)
        {
            Texture* t = 0;
            if(s < m->m_MeshInfos.size())
            {
                int w,h,ch;
                if(m->m_MeshInfos[s].TSNR_ID != -1)
                {
                    int TexID = m->m_TXSTs[m->m_TSNRs[m->m_MeshInfos[s].TSNR_ID].TXST_ID].TextureID;
                    if(TexID < m->m_Textures.size())
                    {
                        Byte* Data = (Byte*)SOIL_load_image(m->m_Textures[TexID].c_str(),&w,&h,&ch,4);
                        
                        if(Data)
                        {
                            t = m_Renderer->GetRasterizer()->CreateTexture();
                            t->CreateTextureb(w,h,Data);
                            for(int x = 0;x < w;x++)
                            {
                                for(int y = 0;y < h;y++)
                                {
                                    Vec4 c = t->GetPixel(Vec2(x,y));
                                    if(c.w < 1.0f && c.w != 0.0f) c.w = 1.0f;
                                    t->SetPixel(Vec2(x,y),c);
                                }
                            }
                            t->UpdateTexture();
                        }
                        
                        delete [] Data;
                    }
                }
            }
            
            SubMesh* sm = m->GetSubMesh(s);
            if(sm->GetVertexCount() != 0)
            {
                //printf("Mesh[%d]: %lu, %lu.\n",s,sm->m_Vertices.size(),sm->m_Indices.size());
                
                vector<Vec2> TexCs;
                vector<Vec3> Verts,Norms;
                vector<Vec4> Colors;
                vector<i32 > Indices;
            
                for(i32 vIdx = 0;vIdx < sm->GetVertexCount();vIdx++)
                {
                    Vec2 t0;
                    Vec3 v0,n0;
                    sm->GetTexCoord(vIdx,&t0.x);
                    sm->GetVertex(vIdx,&v0.x);
                    sm->GetNormal(vIdx,&n0.x);
                    
                    TexCs .push_back(t0);
                    Verts .push_back(v0);
                    Norms .push_back(n0);
                    Colors.push_back(Vec4(1,1,1,1));
                }
                if(sm->GetIndexCount() != 0)
                {
                    for(i32 idx = 0;idx < sm->GetIndexCount();idx++)
                    {
                        i32 I = sm->GetIndex(idx);
                        Indices.push_back(I);
                    }
                    
                    Mesh* M = new Mesh();
                    if(Indices.size() != 0) M->SetIndexBuffer   (Indices.size(),&Indices[0]  );
                    M->SetVertexBuffer  (Verts  .size(),&Verts  [0].x);
                    M->SetNormalBuffer  (Norms  .size(),&Norms  [0].x);
                    M->SetTexCoordBuffer(TexCs  .size(),&TexCs  [0].x);
                    M->PrimitiveType = PT_TRIANGLE_STRIP;
                    
                    RenderObject* Obj = m_Renderer->CreateRenderObject(ROT_MESH);
                    
                    Mat4 T = Translation(*(Vec3*)&a->Position);
                    Mat4 R = Rotation(a->Rotation.y,a->Rotation.z,a->Rotation.x);
                    Mat4 S = Scale(*(Vec3*)&a->Scale);
                    Obj->SetTransform(T * R * S);
                
                    Material* Mat = m_Renderer->CreateMaterial();
                    Mat->SetShader(m_Shdr);
                    Mat->SetMap(Material::MT_DIFFUSE,t);
                    Obj->SetMesh(M,Mat);
                    m_Renderer->GetScene()->AddRenderObject(Obj);
                    
                    m_Actors[m_ActorDefs.size()].push_back(Obj);
                    Obj->SetObjectID(m_ActorDefs.size());
                    
                    m_Meshes.push_back(Obj);
                }
                else if(sm->GetChunkCount() != 0)
                {
                    for(i32 ch = 0;ch < sm->GetChunkCount();ch++)
                    {
                        MeshChunk* Chunk = sm->GetChunk(ch);
                        
                        if(ch < m->m_MTRLs.size())
                        {
                            int w,h,chnl;
                            if(m->m_MTRLs[ch].Unk4 >= 0 && m->m_MTRLs[ch].Unk4 < m->m_TSNRs.size())
                            {
                                int TexID = m->m_TXSTs[m->m_TSNRs[m->m_MTRLs[ch].Unk4].TXST_ID].TextureID;
                                if(TexID < m->m_Textures.size())
                                {
                                    Byte* Data = (Byte*)SOIL_load_image(m->m_Textures[TexID].c_str(),&w,&h,&chnl,4);
                                    
                                    if(Data)
                                    {
                                        t = m_Renderer->GetRasterizer()->CreateTexture();
                                        t->CreateTextureb(w,h,Data);
                                        for(int x = 0;x < w;x++)
                                        {
                                            for(int y = 0;y < h;y++)
                                            {
                                                Vec4 c = t->GetPixel(Vec2(x,y));
                                                if(c.w < 1.0f && c.w != 0.0f) c.w = 1.0f;
                                                t->SetPixel(Vec2(x,y),c);
                                            }
                                        }
                                        t->UpdateTexture();
                                    }
                                    
                                    delete [] Data;
                                }
                            }
                        }
                        
                        Indices.clear();
                        for(i32 idx = 0;idx < Chunk->GetIndexCount();idx++)
                        {
                            i32 I = Chunk->GetIndex(idx);
                            Indices.push_back(I);
                        }
                    
                        Mesh* M = new Mesh();
                        if(Indices.size() != 0) M->SetIndexBuffer   (Indices.size(),&Indices[0]  );
                        M->SetVertexBuffer  (Verts  .size(),&Verts  [0].x);
                        M->SetNormalBuffer  (Norms  .size(),&Norms  [0].x);
                        M->SetTexCoordBuffer(TexCs  .size(),&TexCs  [0].x);
                        M->PrimitiveType = PT_TRIANGLE_STRIP;
                        
                        RenderObject* Obj = m_Renderer->CreateRenderObject(ROT_MESH);
                        
                        Mat4 T = Translation((*(Vec3*)&a->Position));
                        Mat4 R = Rotation(a->Rotation.y,a->Rotation.x,a->Rotation.z);
                        Mat4 S = Scale(*(Vec3*)&a->Scale);
                        Obj->SetTransform(T * R * S);
                    
                        Material* Mat = m_Renderer->CreateMaterial();
                        Mat->SetShader(m_Shdr);
                        Mat->SetMap(Material::MT_DIFFUSE,t);
                        Obj->SetMesh(M,Mat);
                        m_Renderer->GetScene()->AddRenderObject(Obj);
                        
                        m_Meshes.push_back(Obj);
                        
                        m_Actors[m_ActorDefs.size()].push_back(Obj);
                        Obj->SetObjectID(m_ActorDefs.size());
                    }
                }
            }
        }
        
        m_ActorDefs.push_back(a);
        m_ActorIsStatic.push_back(IsStatic);
    }
    Mat4 T4_Viewer::GetActorTransform(i32 AID) const
    {
        Mat4 T = Translation(*(Vec3*)&m_ActorDefs[AID]->Position);
        Mat4 R = Rotation(m_ActorDefs[AID]->Rotation.y,m_ActorDefs[AID]->Rotation.z,m_ActorDefs[AID]->Rotation.x);
        Mat4 S = Scale(*(Vec3*)&m_ActorDefs[AID]->Scale);
        return T * R * S;
    }
    void T4_Viewer::SetActorPosition(i32 AID,const Vec3& Pos)
    {
        m_ActorDefs[AID]->Position = *(ActorVec3*)&Pos;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    void T4_Viewer::SetActorRotation(i32 AID,const Vec3& Rot)
    {
        m_ActorDefs[AID]->Rotation = *(ActorVec3*)&Rot;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    void T4_Viewer::SetActorScale   (i32 AID,const Vec3& Scl)
    {
        m_ActorDefs[AID]->Scale    = *(ActorVec3*)&Scl;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    void T4_Viewer::Initialize()
    {
        InitGUI         ();
        InitFlyCamera   ();
        InitDebugDisplay();
		
        m_Rasterizer->SetClearColor(Vec4(0,0,0,1));
        SetFPSPrintFrequency(0.5f);
        SetTargetFrameRate(100000);
        
        /*
         * Generate material
         */
        
        m_ShaderGenerator->Reset();
        m_ShaderGenerator->SetShaderVersion   (330);
        m_ShaderGenerator->SetAllowInstancing (false);
        
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
        m_ShaderGenerator->SetUniformInput    (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        m_ShaderGenerator->SetTextureInput    (Material::MT_DIFFUSE, true);
        
        m_ShaderGenerator->SetAttributeInput  (ShaderGenerator::IAT_POSITION,true);
        m_ShaderGenerator->SetAttributeInput  (ShaderGenerator::IAT_TEXCOORD,true);
        m_ShaderGenerator->SetAttributeInput  (ShaderGenerator::IAT_NORMAL  ,true);
        
        m_ShaderGenerator->SetAttributeOutput (ShaderGenerator::IAT_POSITION,true);
        m_ShaderGenerator->SetAttributeOutput (ShaderGenerator::IAT_TEXCOORD,true);
        m_ShaderGenerator->SetAttributeOutput (ShaderGenerator::IAT_NORMAL  ,true);
        
        m_ShaderGenerator->SetFragmentOutput  (ShaderGenerator::OFT_COLOR   ,true);
        m_ShaderGenerator->SetFragmentOutput  (ShaderGenerator::OFT_POSITION,true);
        m_ShaderGenerator->SetFragmentOutput  (ShaderGenerator::OFT_NORMAL  ,true);
        m_ShaderGenerator->SetFragmentOutput  (ShaderGenerator::OFT_CUSTOM0 ,true);
        
        
        m_ShaderGenerator->AddVertexModule  ("[PtSz]gl_PointSize = 5.0;[/PtSz]",0);
        m_ShaderGenerator->AddFragmentModule("[AlphaTest]if(sColor.a < 0.1) discard;[/AlphaTest]",0);
        m_ShaderGenerator->AddFragmentModule(
        "[SetCustom0]"
        "int x = (u_ObjectID & int(0xff000000)) >> 24;\n"
        "int y = (u_ObjectID & int(0x00ff0000)) >> 16;\n"
        "int z = (u_ObjectID & int(0x0000ff00)) >>  8;\n"
        "int w = (u_ObjectID & int(0x000000ff)) >>  0;\n"
        "float Inv255 = 1.0 / 255.0;\n"
        "vec4 sCustom0 = vec4(float(w) * Inv255,float(z) * Inv255,float(y) * Inv255,float(x) * Inv255);\n"
        "[/SetCustom0]",1);
        
        m_ShaderGenerator->SetLightingMode(ShaderGenerator::LM_PASS);
        
        m_Shdr = m_ShaderGenerator->Generate();
        
        //Light pass materials
        DeferredRenderer* r = (DeferredRenderer*)m_Renderer;
        
        Material* Pt = m_Renderer->CreateMaterial();
        Pt->LoadMaterial(Load("Silk/PointLight.mtrl"));
        Material* Sp = m_Renderer->CreateMaterial();
        Sp->LoadMaterial(Load("Silk/SpotLight.mtrl"));
		Material* Dr = m_Renderer->CreateMaterial();
        Dr->LoadMaterial(Load("Silk/DirectionalLight.mtrl"));
        Material* Final = m_Renderer->CreateMaterial();
        Final->LoadMaterial(Load("Silk/FinalDeferredPassNoFxaa.mtrl"));
        
        /*
        InitSSAO();
        SetSSAOIntensity(1.0f);
        SetSSAONoiseScale(4.0f);
        SetSSAORadius(0.1f);
        */
        
        r->SetPointLightMaterial(Pt);
        r->SetSpotLightMaterial(Sp);
        r->SetDirectionalLightMaterial(Dr);
        r->SetFinalPassMaterial(Final);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        /*
         * Load static meshes
         */
        for(i32 i = 0;i < m_ATR.GetActorCount();i++) AddActorToScene(m_ATR.GetActor(i),true);
        
        /*
         * Load dynamic meshes
         */
        ActorInstances* ATI = m_ATR.GetActors();
        if(ATI)
        {
            for(i32 i = 0;i < ATI->m_Actors.size();i++) AddActorToScene(ATI->m_Actors[i],false);
        }
        
        //m_DebugDraw->SetDebugDisplay(DebugDrawer::DD_OBB,true);
        
        Light* L = AddLight(LT_DIRECTIONAL,Vec3(0,10,0))->GetLight();
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 1.3f;
    }

    void T4_Viewer::Run()
    {
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
        
        Scalar a = 12.0f;
        i32 SelectedIdx = -1;
        Scalar sDist = 0.0f;
        bool BtnDown = false;
        while(IsRunning())
        {
            Mat4 T = (m_Camera->GetTransform() * m_Camera->GetProjection().Inverse()).Transpose();
            
            Vec3 t[4] = { Vec3(-0.01,0.0,0.0), Vec3(0.01,0.0,0.0), Vec3(0.0,-0.01,0.0), Vec3(0.0,0.01,0.0) };
            m_DebugDraw->Line(T * t[0],T * t[1],Vec4(1,1,1,1));
            m_DebugDraw->Line(T * t[2],T * t[3],Vec4(1,1,1,1));
            if(m_InputManager->IsButtonDown(BTN_LEFT_MOUSE))
            {
                if(!BtnDown)
                {
                    Texture* t = m_Renderer->GetSceneOutput()->GetAttachment(ShaderGenerator::OFT_CUSTOM0);
                    t->AcquireFromVRAM();
                    Vec4 ObjID = t->GetPixel(m_Renderer->GetRasterizer()->GetContext()->GetResolution() * 0.5f);
                    char v[4] =
                    {
                        (char)(i32)(ObjID.x * 255.0f),
                        (char)(i32)(ObjID.y * 255.0f),
                        (char)(i32)(ObjID.z * 255.0f),
                        (char)(i32)(ObjID.w * 255.0f),
                    };
                    memcpy(&SelectedIdx,&v,sizeof(i32));
                    if(SelectedIdx >= 0)
                    {
                        sDist = (GetActorTransform(SelectedIdx).GetTranslation() - m_CamPos).Magnitude();
                    }
                }
                
                if(SelectedIdx >= 0)
                {
                    Vec4 mColor = Vec4(1,0,0,1);
                    if(!m_ActorIsStatic[SelectedIdx])
                    {
                        mColor = Vec4(0.5f,0.5f,1.0f,1.0f);
                        SetActorPosition(SelectedIdx,m_CamPos + m_Camera->GetTransform().GetZ() * -sDist);
                    }
                    
                    for(i32 i = 0;i < m_Actors[SelectedIdx].size();i++)
                    {
                        m_Renderer->GetDebugDrawer()->OBB(m_Actors[SelectedIdx][i]->GetBoundingBox(),Vec4(1,0,0,1));
                        m_Renderer->GetDebugDrawer()->DrawMesh(m_Actors[SelectedIdx][i]->GetTransform(),m_Actors[SelectedIdx][i]->GetMesh(),mColor);
                    }
                }
                BtnDown = true;
            }
            else BtnDown = false;
            
            a += GetDeltaTime() * 0.01f;
            //m_DebugDraw->Line(Vec3(0,0,0),Vec3(0,10,0),Vec4(1,1,1,1));
            m_Lights[0]->SetTransform(Rotation(Vec3(0,0,1),90 + (a * 7.5f)) * Rotation(Vec3(1,0,0),-90.0f));
        }
        
        for(i32 i = 0;i < m_ActorDefs.size();i++)
        {
            m_ActorDefs[i]->SaveTransform();
        }
    }
    void T4_Viewer::Shutdown()
    {
    }
};
