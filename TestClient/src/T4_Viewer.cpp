
/*
 *  T4_Viewer.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#include <T4_Viewer.h>
#include <T4/SOIL/SOIL.h>
#include <T4/ConvertActor.h>

#include <UI/UIText.h>

#include <UIElements/TabPanel.h>

namespace TestClient
{
    T4_Viewer::T4_Viewer(i32 ArgC,char* ArgV[])
    {
        //Check for arguments
        if(ArgC < 3)
        {
			//-d currently does nothing
            printf("Usage: T4_ATR <Turok base folder> <ATR file> (Optional -d <Output folder>)");
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
        Turok4::SetTurokDirectory(TurokDir);
        m_ATR = new Turok4::ATRFile();
        if(!m_ATR->Load(File)) { delete m_ATR; return false; }
        m_Filename = m_ATR->GetActors()->GetFile();
        return true;
    }
    void T4_Viewer::AddActorsToScene()
    {
        for(i32 i = 0;i < m_ActorDefs.size();i++)
        {
            m_Actors.push_back(SilkObjectVector());
            LoadActorToEngine(m_Renderer,m_Shdr,GetActorTransform(i),i,m_ActorDefs[i],m_Actors[i]);
            
            for(i32 a = 0;a < m_Actors[i].size();a++) m_Meshes.push_back(m_Actors[i][a]);
            
            m_ActorIsStatic.push_back(m_ActorDefs[i]->GetDef() == 0);
        }
        //DebugPrintActorInfo();
    }
    Mat4 T4_Viewer::GetActorTransform(i32 AID) const
    {
        if(!m_ActorDefs[AID]->GetDef()) return Mat4::Identity;
        
        Mat4 T = Translation(*(Vec3*)&m_ActorDefs[AID]->GetDef()->Position);
        Mat4 R = RotationX(m_ActorDefs[AID]->GetDef()->Rotation.x) *
                 RotationY(m_ActorDefs[AID]->GetDef()->Rotation.y) *
                 RotationZ(m_ActorDefs[AID]->GetDef()->Rotation.z);
        Mat4 S = Scale(*(Vec3*)&m_ActorDefs[AID]->GetDef()->Scale);
        return T * R * S;
    }
    void T4_Viewer::SetActorPosition(i32 AID,const Vec3& Pos)
    {
        if(!m_ActorDefs[AID]->GetDef()) return;
        m_ActorDefs[AID]->GetDef()->Position = *(Turok4::ActorVec3*)&Pos;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    void T4_Viewer::SetActorRotation(i32 AID,const Vec3& Rot)
    {
        if(!m_ActorDefs[AID]->GetDef()) return;
        m_ActorDefs[AID]->GetDef()->Rotation.x = Rot.x;
        m_ActorDefs[AID]->GetDef()->Rotation.y = Rot.y;
        m_ActorDefs[AID]->GetDef()->Rotation.z = Rot.z;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    void T4_Viewer::SetActorScale   (i32 AID,const Vec3& Scl)
    {
        if(!m_ActorDefs[AID]->GetDef()) return;
        m_ActorDefs[AID]->GetDef()->Scale    = *(Turok4::ActorVec3*)&Scl;
        Mat4 t = GetActorTransform(AID);
        for(i32 i = 0;i < m_Actors[AID].size();i++) m_Actors[AID][i]->SetTransform(t);
    }
    Scalar T4_Viewer::GetActorRadius(i32 AID) const
    {
        Vec3 MaxExtents = Vec3(FLT_MIN,FLT_MIN,FLT_MIN);
        for(i32 i = 0;i < m_Actors[AID].size();i++)
        {
            Vec3 aMax = m_Actors[AID][i]->GetBoundingBox().GetLocalAABB().GetExtents();
            if(aMax.x > MaxExtents.x) MaxExtents.x = aMax.x;
            if(aMax.y > MaxExtents.y) MaxExtents.y = aMax.y;
            if(aMax.z > MaxExtents.z) MaxExtents.z = aMax.z;
        }
        return MaxExtents.Magnitude();
    }
    void T4_Viewer::Initialize()
    {
        InitGUI         ();
		InitRenderGUI   ();
        InitFlyCamera   ();
        InitDebugDisplay();
		
        m_Rasterizer->SetClearColor(Vec4(0,0,0,1));
        SetFPSPrintFrequency(10.0f);
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
        Final->LoadMaterial(Load("Silk/FinalDeferredPassNoFXAA.mtrl"));
        
		/*
        InitSSAO();
        SetSSAOIntensity(1.0f);
        SetSSAONoiseScale(4.0f);
        SetSSAORadius(0.2f);
        */

        r->SetPointLightMaterial(Pt);
        r->SetSpotLightMaterial(Sp);
        r->SetDirectionalLightMaterial(Dr);
        r->SetFinalPassMaterial(Final);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        
        /*
         * Load meshes
         */
        
        Turok4::ATIFile* ATI = m_ATR->GetActors();
        m_ActorDefs.push_back(m_ATR->GetActor());
        if(ATI)
        {
            for(i32 i = 0;i < ATI->GetActorCount();i++) m_ActorDefs.push_back(ATI->GetActorDef(i)->Actor->GetActor());
        }
        else
        {
            printf("Not a level ATR file.\n");
            m_DoShutdown = true;
        }
        
        AddActorsToScene();
        
        //m_DebugDraw->SetDebugDisplay(DebugDrawer::DD_OBB,true);
        
        Light* L = AddLight(LT_DIRECTIONAL,Vec3(0,10,0))->GetLight();
        L->m_Color = Vec4(1,1,1,1);
        L->m_Power = 1.3f;
        
        m_Toolbar = new UIPanel(Vec2(m_Renderer->GetRasterizer()->GetContext()->GetResolution().x,20.0f));
        m_Toolbar->SetBackgroundColor(Vec4(0.25f,0.25f,0.25f,0.5f));
        m_Toolbar->SetPosition(Vec3(0,0,0));
        m_UIManager->AddElement(m_Toolbar);
        m_ToolbarVelocity = -1.0f;
        m_ToolbarTranslation = 0.0f;
        m_ToolbarButtonDown = m_ActorPanelButtonDown = false;
        
        m_ActorPanel     = new ActorPanel(m_UIManager,m_InputManager);
        m_LoadActorPanel = new LoadActorPanel(GetPreferredInitResolution() * 0.9f,m_UIManager,m_InputManager);
        m_LoadActorPanel->SetPosition(Vec3((GetPreferredInitResolution().x * 0.5f) - (m_LoadActorPanel->GetBounds()->GetDimensions().x * 0.5f),
                                           (GetPreferredInitResolution().y * 0.5f) - (m_LoadActorPanel->GetBounds()->GetDimensions().y * 0.5f),0.0f));
		m_LoadActorPanel->SetEnabled(false);
        
        m_TransformTool = new TransformTool(this);
    }

    void T4_Viewer::Run()
    {
        m_TaskManager->GetTaskContainer()->SetAverageTaskDurationSampleCount(10);
        m_TaskManager->GetTaskContainer()->SetAverageThreadTimeDifferenceSampleCount(10);
        
        Scalar rt = 12.0f;
        i32 SelectedIdx = -1;
        bool BtnDown = false;
        bool APButtonDown = false;
		bool L_Released = true;
        
        m_TransformTool->ToggleTranslate();
        m_TransformTool->ToggleRotate   ();
        
        while(IsRunning())
        {
            Mat4 T = (m_Camera->GetTransform() * m_Camera->GetProjection().Inverse()).Transpose();
            
            Vec3 t[4] = { Vec3(-0.01,0.0,0.0), Vec3(0.01,0.0,0.0), Vec3(0.0,-0.01,0.0), Vec3(0.0,0.01,0.0) };
            m_DebugDraw->Line(T * t[0],T * t[1],Vec4(1,1,1,1));
            m_DebugDraw->Line(T * t[2],T * t[3],Vec4(1,1,1,1));
            
            if(m_InputManager->IsButtonDown(BTN_LEFT_MOUSE) && !m_LoadActorPanel->IsEnabled())
            {
                if(!BtnDown)
                {
                    Texture* t = m_Renderer->GetSceneOutput()->GetAttachment(ShaderGenerator::OFT_CUSTOM0);
                    t->AcquireFromVRAM();
                    
                    Vec2 cPos = m_Renderer->GetRasterizer()->GetContext()->GetResolution() * 0.5f;
                    if(m_Toolbar->IsEnabled())
                    {
                        cPos = m_InputManager->GetCursorPosition();
                        if(m_ActorPanel->IsEnabled())
                        {
                            if(cPos.x < m_ActorPanel->GetPosition().x) cPos.x = -1.0f;
                            else
                            {
                                Vec2   r = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
                                Scalar p = m_ActorPanel->GetPosition().x + m_ActorPanel->GetBounds()->GetDimensions().x;
                                cPos.x = r.x * ((cPos.x - p) / (r.x - p));
                            }
                        }
                        cPos.y = m_Rasterizer->GetContext()->GetResolution().y - cPos.y;
                    }
                    if(cPos.x >= 0.0f)
                    {
                        Vec4 ObjID = t->GetPixel(cPos);
                        char v[4] =
                        {
                            (char)(i32)(ObjID.x * 255.0f),
                            (char)(i32)(ObjID.y * 255.0f),
                            (char)(i32)(ObjID.z * 255.0f),
                            (char)(i32)(ObjID.w * 255.0f),
                        };
                        i32 sIdx = SelectedIdx;
                        memcpy(&SelectedIdx,&v,sizeof(i32));
                        if(SelectedIdx >= 0)
                        {
                            if(!m_TransformTool->OnObjClick(SelectedIdx))
                            {
                                m_ActorPanel->SetActor(m_ActorDefs[SelectedIdx]);
                                
                                Mat4 t = GetActorTransform(SelectedIdx);
                                m_TransformTool->SetTransform(t);
                                m_TransformTool->SetMinDisplaySize(GetActorRadius(SelectedIdx) * 2.0f);
                            }
                            else SelectedIdx = sIdx;
                        }
                    }
                }
                BtnDown = true;
            }
            else
            {
                BtnDown = false;
                m_TransformTool->OnClickUp();
            }
            
            if(SelectedIdx >= 0 && SelectedIdx < m_Actors.size())
            {
                Vec4 mColor = Vec4(1,0,0,0.45f);
                if(m_TransformTool->IsEnabled() && !m_ActorIsStatic[SelectedIdx])
                {
                    SetActorPosition(SelectedIdx,m_TransformTool->GetTransform().GetTranslation());
                    SetActorRotation(SelectedIdx,m_TransformTool->GetRotation ().ToEuler       ());
                    //SetActorScale   (SelectedIdx,m_TransformTool->GetTransform().GetScale());
					
					mColor = Vec4(0.5f,0.5f,1.0f,0.45f);
                }

                for(i32 i = 0;i < m_Actors[SelectedIdx].size();i++)
                {
                    //m_Renderer->GetDebugDrawer()->OBB(m_Actors[SelectedIdx][i]->GetBoundingBox(),Vec4(1,0,0,1));
                    m_Renderer->GetDebugDrawer()->DrawMesh(m_Actors[SelectedIdx][i]->GetTransform(),m_Actors[SelectedIdx][i]->GetMesh(),mColor);
                }
            }

			if(m_InputManager->GetButtonDownDuration(BTN_L) != -1.0f)
			{
				if(L_Released) m_LoadActorPanel->SetEnabled(!m_LoadActorPanel->IsEnabled());
				L_Released = false;
			}
			else L_Released = true;
            
            if(m_InputManager->GetCursorDelta().MagnitudeSq() > 1.0f) m_TransformTool->OnCursorMove();
            
            rt += GetDeltaTime();
            //m_DebugDraw->Line(Vec3(0,0,0),Vec3(0,10,0),Vec4(1,1,1,1));
            m_Lights[0]->SetTransform(Rotation(Vec3(0,0,1),90 + (rt * 7.5f)) * Rotation(Vec3(1,0,0),-90.0f));
            
            m_TransformTool->SetDisplayScale(((m_Camera->GetTransform().GetTranslation() - m_TransformTool->GetTransform().GetTranslation()).Magnitude() * 0.1f));
            
            UpdateUI();
        }
        
        //m_ATR->GetActors()->Save(m_Filename);
    }
    void T4_Viewer::UpdateUI()
    {
        Scalar dt = GetDeltaTime();
        
        if(m_InputManager->GetButtonDownDuration(BTN_TOGGLE) > 0.0f && !m_ToolbarButtonDown)
        {
            m_ToolbarButtonDown = true;
            if(m_ToolbarTranslation == 1.0f) m_ToolbarVelocity = -2.0f;
            else if(m_ToolbarTranslation == 0.0f)
            {
                m_ToolbarVelocity = 2.0f;
                m_Cursor->SetEnabled(true);
                m_Toolbar->SetEnabled(true);
                m_FlyCameraEnabled = false;
            }
        }
        else if(m_InputManager->GetButtonDownDuration(BTN_TOGGLE) != -1.0f) m_ToolbarButtonDown = false;
        
        if(m_InputManager->GetButtonDownDuration(BTN_P) > 0.0f && !m_ActorPanelButtonDown)
        {
            m_ActorPanelButtonDown = true;
            m_ActorPanel->Toggle();
        }
        else if(m_InputManager->GetButtonDownDuration(BTN_P) != -1.0f) m_ActorPanelButtonDown = false;
        
        if(m_ToolbarVelocity != 0.0f)
        {
            m_ToolbarTranslation += m_ToolbarVelocity * dt;
            
            if(m_ToolbarTranslation > 1.0f)
            {
                m_ToolbarTranslation = 1.0f;
                m_ToolbarVelocity = 0.0f;
            }
            
            if(m_ToolbarTranslation < 0.0f)
            {
                m_ToolbarTranslation = 0.0f;
                m_ToolbarVelocity = 0.0f;
                
                m_FlyCameraEnabled = true;
                m_Cursor->SetEnabled(false);
                m_Toolbar->SetEnabled(false);
            }
            
            m_Toolbar->SetPosition(Vec3(0.0f,-20.0f + (m_ToolbarTranslation * 20.0f),0.0f));
            m_ActorPanel->SetPosition(Vec3(m_ActorPanel->GetPosition().x,m_Toolbar->GetPosition().y + m_Toolbar->GetBounds()->GetDimensions().y,0.0f));
        }
    }
    void T4_Viewer::Shutdown()
    {
    }
};
