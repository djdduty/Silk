#include <UI/UI.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>

namespace Silk
{
    UIManager::UIManager(Renderer* r) : m_Renderer(r), m_ViewScale(1.0f,1.0f), m_Resolution(0.0f,0.0f), m_ViewNeedsUpdate(true),
                                        m_Camera(0), m_View(0), m_DefaultShader(0), m_DefaultTextureShader(0), m_DefaultTextShader(0),
                                        m_RenderUniforms(0), m_MouseLeftID(-1), m_MouseMiddleID(-1), m_MouseRightID(-1)
    {
    }
    UIManager::~UIManager()
    {
        if(m_Camera) delete m_Camera;
    }
    
    void UIManager::Initialize(i32 ButtonCount)
    {
        m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        Vec2 hRes = m_Resolution * 0.5f;
        m_Camera = new Camera(-hRes.x,hRes.x,-hRes.y,hRes.y,0.0f,1.0f);
        m_View = m_Renderer->GetRasterizer()->CreateTexture();
        m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
        
        ShaderGenerator* Gen = m_Renderer->GetShaderGenerator();
        Gen->Reset();
        
        Gen->SetShaderVersion(330);
        Gen->SetAllowInstancing(false);
        Gen->SetAllowInstancedTextureMatrix(false);
        
        Gen->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        Gen->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        Gen->SetUniformInput(ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);

        Gen->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
        Gen->SetAttributeInput (ShaderGenerator::IAT_COLOR   ,true);
        Gen->SetAttributeInput (ShaderGenerator::IAT_TEXCOORD,true);
        
        Gen->SetAttributeOutput(ShaderGenerator::IAT_COLOR   ,true);
        Gen->SetAttributeOutput(ShaderGenerator::IAT_TEXCOORD,true);
        Gen->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
        
        Gen->SetLightingMode(ShaderGenerator::LM_FLAT);
        
        m_DefaultShader = Gen->Generate();
        
        Gen->SetTextureInput(Material::MT_DIFFUSE,true);
        m_DefaultTextureShader = Gen->Generate();
        
        Gen->AddFragmentModule(const_cast<CString>("[Alpha]float Alpha = smoothstep(0.5 - 0.25,0.5 + 0.25,texture(u_DiffuseMap,sTexCoord).a) * o_Color.a;[/Alpha]"),0);
        Gen->AddFragmentModule(const_cast<CString>("[AlphaTest]if(Alpha < 0.0001) discard;[/AlphaTest]"),1);
        Gen->AddFragmentModule(const_cast<CString>("[SetColor]vec4 sColor = vec4(o_Color.rgb,Alpha);[/SetColor]"),2);
        m_DefaultTextShader    = Gen->Generate();
        
        Gen->Reset();
        
        for(i32 i = 0;i < ButtonCount;i++) m_ButtonDurations.push_back(-1.0f);
    }
    void UIManager::OnButtonDown(i32 ButtonID)
    {
        m_ButtonDurations[ButtonID] = 0.0f;
        
        //Should something go here?
        if(ButtonID == m_MouseLeftID)
        {
        }
        else if(ButtonID == m_MouseRightID)
        {
        }
        else if(ButtonID == m_MouseMiddleID)
        {
        }
    }
    void UIManager::OnButtonUp(i32 ButtonID)
    {
        m_ButtonDurations[ButtonID] = -1.0f;
    }
    void UIManager::SetCursorPosition(const Vec2& p)
    {
        m_RealCursorPosition = p;
        
        //Lock cursor to window
        Vec2 Temp = m_RealCursorPosition;
        if(p.x < 0             ) Temp.x = 0             ;
        if(p.x > m_Resolution.x) Temp.x = m_Resolution.x;
        if(p.y < 0             ) Temp.y = 0             ;
        if(p.y > m_Resolution.y) Temp.y = m_Resolution.y;
        
        //Transform to normalized window coordinates
        if(Temp.x != 0) Temp.x /= m_Resolution.x;
        if(Temp.y != 0) Temp.y /= m_Resolution.y;
        
        //Transform to "virtual" window coordinates
        Vec4 Ortho  = m_Camera->GetOrthoRect();
        Vec3 CamPos = m_Camera->GetTransform().GetTranslation();
        
        m_CursorPosition = CamPos.xy() + Ortho.xy() + (Ortho.zw() * Temp);
    }
    void UIManager::Update(Scalar dt)
    {
        for(i32 i = 0;i < m_Elements.size();i++) m_Elements[i]->_Update(dt);
    }
    void UIManager::Render(Scalar dt,PRIMITIVE_TYPE PrimType)
    {
        //Update button times
        for(i32 i = 0;i < m_ButtonDurations.size();i++)
        {
            if(m_ButtonDurations[i] != -1.0f) m_ButtonDurations[i] += dt;
        }
        
        //Update texture and projection if resolution changes
        Vec2 cRes = m_Renderer->GetRasterizer()->GetContext()->GetResolution();;
        if(m_Resolution.x != cRes.x || m_Resolution.y != cRes.y)
        {
            m_Resolution = cRes;
            m_Camera->SetOrthographic(m_Resolution);
            m_ViewNeedsUpdate = true;
            m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
            m_View->UpdateTexture();
        }
        
        if(!m_ViewNeedsUpdate) return;
        //m_ViewNeedsUpdate = false;
        
        //Set active camera
        Camera* Cam = m_Renderer->GetScene()->GetActiveCamera();
        m_Renderer->GetScene()->SetActiveCamera(m_Camera);
        m_Renderer->UpdateUniforms();
        
        //UI culling
        ObjectList l;
        l.SetIndexed(false);
        
        //Depth sorting
        vector<UIElement*> DepthSorted;
        vector<UIElement*> Temp(m_Elements);
        while(DepthSorted.size() != m_Elements.size())
        {
            for(i32 i = 0;i < Temp.size();i++)
            {
                Scalar Tz = Temp[i]->m_Render->GetTransform().GetTranslation().z;
                if(DepthSorted.size() == 0)
                {
                    DepthSorted.push_back(Temp[i]);
                    Temp.erase(Temp.begin() + i);
                    break;
                }
                
                for(i32 j = 0;j < DepthSorted.size();j++)
                {
                    Scalar Dz = DepthSorted[j]->m_Render->GetTransform().GetTranslation().z;
                    if(Tz > Dz)
                    {
                        if(j < DepthSorted.size() - 1)
                        {
                            if(Tz < Dz)
                            {
                                DepthSorted.insert(DepthSorted.begin() + j,Temp[i]);
                                Temp.erase(Temp.begin() + i);
                                break;
                            }
                            else continue;
                        }
                        else
                        {
                            DepthSorted.push_back(Temp[i]);
                            Temp.erase(Temp.begin() + i);
                            break;
                        }
                    }
                    else if(Tz < Dz)
                    {
                        if(j != 0) DepthSorted.insert(DepthSorted.begin() + (j - 1),Temp[i]);
                        else DepthSorted.insert(DepthSorted.begin(),Temp[i]);
                        Temp.erase(Temp.begin() + i);
                        break;
                    }
                    else
                    {
                        DepthSorted.insert(DepthSorted.begin() + j,Temp[i]);
                        Temp.erase(Temp.begin() + i);
                        break;
                    }
                }
            }
        }
        
        //for(i32 i = 0;i < DepthSorted.size();i++)
        for(i32 i = DepthSorted.size() - 1;i >= 0;i--)
        {
            //To do: culling
            l.AddObject(DepthSorted[i]->m_Render);
        }
        
        //Render all UI to texture
        m_View->EnableRTT();
        
        i32 ShaderCount = l.GetShaderCount();
        
        SilkObjectVector MeshesRendered;
        for(i32 i = 0;i < ShaderCount;i++)
        {
            Shader* Shader = l.GetShader(i);
            if(!Shader) continue;
            
            Shader->Enable();
            
            SilkObjectVector Meshes = l.GetShaderMeshList(i);
            for(i32 m = 0;m < Meshes.size();m++)
            {
                RenderObject* Obj = Meshes[m];
            
                if(Obj->m_Mesh && Obj->m_Material && Obj->m_Enabled)
                {
                    //Pass material uniforms
                    Material* Mat = Obj->GetMaterial();
                    Shader->UseMaterial(Mat);
                    
                    //Pass object uniforms
                    if(Shader->UsesUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS))
                    {
                        Obj->UpdateUniforms();
                        Shader->PassUniforms(Obj->GetUniformSet()->GetUniforms());
                    }
                    
                    //To do: Batching
                    i32 Count = 0;
                    if(Obj->m_Mesh->IsIndexed()) Count = Obj->m_Mesh->GetIndexCount();
                    else Count = Obj->m_Mesh->GetVertexCount();
                    
                    Obj->m_Object->Render(Obj,PrimType,0,Count);
                    
                    
                    i32 vc = Obj->m_Mesh->GetVertexCount();
                    i32 tc = 0;
                    if(PrimType == PT_TRIANGLES     ) tc = vc / 3;
                    if(PrimType == PT_TRIANGLE_STRIP
                    || PrimType == PT_TRIANGLE_FAN  ) tc = vc - 2;
                    
                    m_Renderer->m_Stats.VisibleObjects++;
                    m_Renderer->m_Stats.VertexCount   += vc;
                    m_Renderer->m_Stats.TriangleCount += tc;
                    m_Renderer->m_Stats.DrawCalls     ++   ;
                    
                    MeshesRendered.push_back(Obj);
                }
            }
            
            Shader->Disable();
        }
        
        for(i32 i = 0;i < MeshesRendered.size();i++)
        {
            MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();
        }
        
        m_View->DisableRTT();
        
        m_Renderer->GetScene()->SetActiveCamera(Cam);
        
        //Do some other stuff with the view
    }
    void UIManager::AddElement(UIElement *Element)
    {
        Element->m_Manager = this;
        m_Elements.push_back(Element);
        Element->m_ID = m_Elements.size() - 1;
    }
    void UIManager::RemoveElement(UIElement *Element)
    {
        if(!Element->m_Manager) return;
        m_Elements.erase(m_Elements.begin() + Element->m_ID);
        for(i32 i = Element->m_ID;i < m_Elements.size();i++)
        {
            m_Elements[i]->m_ID--;
        }
        Element->m_ID = -1;
    }
};
