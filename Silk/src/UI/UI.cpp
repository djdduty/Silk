#include <UI/UI.h>

#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    UIManager::UIManager(Renderer* r) : m_Renderer(r), m_ViewScale(1.0f,1.0f), m_Resolution(0.0f,0.0f), m_ViewNeedsUpdate(true),
                                        m_Camera(0), m_View(0), m_DefaultShader(0), m_DefaultTextureShader(0), m_DefaultTextShader(0),
                                        m_RenderUniforms(0), m_Font(0)
    {
    }
    UIManager::~UIManager()
    {
        if(m_Camera) delete m_Camera;
        if(m_Font) delete m_Font;
    }
    
    void UIManager::Initialize()
    {
        m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        m_Camera = new Camera(-m_Resolution.x/2,m_Resolution.x/2,-m_Resolution.y/2,m_Resolution.y/2,0.0f,1.0f);
        m_View = m_Renderer->GetRasterizer()->CreateTexture();
        m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
        m_View->UpdateTexture();
        
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
        Gen->AddFragmentModule(const_cast<CString>("[SetColor]vec4 sColor = u_Diffuse;[/SetColor]"),2);
        m_DefaultShader = Gen->Generate();
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
        
        Gen->SetTextureInput(Material::MT_DIFFUSE,true);
        m_DefaultTextureShader = Gen->Generate();
        
        Gen->AddFragmentModule(const_cast<CString>("[Alpha]float Alpha = smoothstep(0.5 - 0.25,0.5 + 0.25,texture(u_DiffuseMap,sTexCoord).a) * o_Color.a;[/Alpha]"),0);
        Gen->AddFragmentModule(const_cast<CString>("[AlphaTest]if(Alpha < 0.0001) discard;[/AlphaTest]"),1);
        Gen->AddFragmentModule(const_cast<CString>("[SetColor]vec4 sColor = vec4(o_Color.rgb,Alpha);[/SetColor]"),2);
        m_DefaultTextShader = Gen->Generate();
        
        Gen->Reset();
        SetTransform(Translation(Vec3(m_Resolution.x * 0.5f,m_Resolution.y * 0.5f,0.0f)));
    }
    void UIManager::Update(Scalar dt)
    {
        for(i32 i = 0;i < m_Elements.size();i++) m_Elements[i]->_Update(dt);
    }
    void UIManager::Render(Scalar dt,PRIMITIVE_TYPE PrimType)
    {
        if(m_Elements.size() == 0) return;
        
        //Update texture and projection if resolution changes
        Vec2 cRes = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        if(m_ViewNeedsUpdate == true)
        {
            //Update texture and projection if resolution changes
            Vec2 cRes = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
            if(m_Resolution.x != cRes.x || m_Resolution.y != cRes.y)
            {
                m_Resolution = cRes;
                m_Camera->SetOrthographic(0, m_Resolution.x, 0, m_Resolution.y, 0.0f, 1.0f);
                m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
                m_View->UpdateTexture();
                SetTransform(Translation(Vec3(m_Resolution.x * 0.5f,m_Resolution.y * 0.5f,0.0f)));
            }
        
            //Set active camera
            Camera* Cam = m_Renderer->GetScene()->GetActiveCamera();
            m_Renderer->GetScene()->SetActiveCamera(m_Camera);
            m_Renderer->UpdateUniforms();

            //TODO: Depth Sorting

            //Render all UI to texture
            m_View->EnableRTT(false);
        
            SilkObjectVector MeshesRendered;
            for(i32 i = m_Elements.size() - 1; i >= 0; i--)
            {
                m_Elements[i]->_Render(PrimType, &MeshesRendered);
            }
        
            for(i32 i = 0;i < MeshesRendered.size();i++)
            {
                MeshesRendered[i]->GetUniformSet()->GetUniforms()->ClearUpdatedUniforms();
            }
        
            m_View->DisableRTT();
        
            m_Renderer->GetScene()->SetActiveCamera(Cam);
            m_ViewNeedsUpdate = false;
        }
        m_Renderer->RenderTexture(m_View);
        
        //Do some other stuff with the view
    }
    void UIManager::AddElement(UIElement *Element)
    {
        Element->m_Manager = this;
        m_Elements.push_back(Element);
        Element->m_ID = m_Elements.size() - 1; //NOTE: Not an actual id, it's just the array index for use in removing
        Element->_Initialize(this);
        m_ViewNeedsUpdate = true;
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
        m_ViewNeedsUpdate = true;
    }
    void UIManager::SetTransform(Mat4 M)
    {
        m_Camera->SetTransform(M);
        m_ViewNeedsUpdate = true;
    }
    void UIManager::SetZClipPlanes(f32 n, f32 f)
    {
        m_Camera->SetZClipPlanes(n, f);
        m_ViewNeedsUpdate = true;
    }
};
