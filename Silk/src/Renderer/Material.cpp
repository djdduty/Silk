#include <Renderer/Material.h>
#include <Renderer/Texture.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Raster/Raster.h>

namespace Silk
{
    static string MaterialMapNames[Material::MT_COUNT] =
    {
        "u_NormalMap"      ,
        "u_DiffuseMap"     ,
        "u_SpecularMap"    ,
        "u_EmissiveMap"    ,
        "u_ParallaxMap"    ,
        "u_DisplacementMap",
        "u_Material0"      ,
        "u_Material1"      ,
        "u_CustomMap0"     ,
        "u_CustomMap1"     ,
        "u_CustomMap2"     ,
        "u_CustomMap3"     ,
        "u_CustomMap4"     ,
        "u_CustomMap5"     ,
        "u_CustomMap6"     ,
        "u_CustomMap7"     ,
        "u_PostProcessOut" ,
        "u_FragColor"      ,
        "u_FragPosition"   ,
        "u_FragNormal"     ,
        "u_FragTangent"    ,
        "u_FragMaterial0"  ,
        "u_FragMaterial1"  ,
        "u_FragCustom0"    ,
        "u_FragCustom1"    ,
        "u_FragCustom2"    ,
        "u_FragCustom3"    ,
        "u_FragCustom4"    ,
        "u_FragCustom5"    ,
        "u_FragCustom6"    ,
        "u_FragCustom7"    ,
        "u_FragLightAccum" ,
    };
    string GetShaderMapName(Material::MAP_TYPE Type)
    {
        return MaterialMapNames[Type];
    }
    
    Material::Material(Renderer* r) : m_RefCount(1), m_Shader(0), m_Renderer(r), m_HasUpdated(true)
    {
        for(i32 i = 0;i < MT_COUNT;i++) m_Maps[i] = 0;
        m_Uniforms = new MaterialUniformSet(m_Renderer);
        m_UserUniforms = 0;
    }
    Material::~Material()
    {
        delete m_Uniforms;
    }
    i32 Material::Destroy()
    {
        m_RefCount--;
        if(m_RefCount == 0)
        {
            m_Renderer->Destroy(this);
            //delete this;
            return 0;
        }
        return m_RefCount;
    }
    void Material::SetMap(MAP_TYPE Type,Texture *Map)
    {
        if(!Map) return;
        if(m_Maps[Type]) m_Maps[Type]->Destroy();
        m_Maps[Type] = Map;
        Map->AddRef();
        m_HasUpdated = true;
    }
    void Material::SetShader(Shader *Shdr)
    {
        if(m_Shader) m_Shader->Destroy();
        m_Shader = Shdr;
        if(m_Shader) m_Shader->AddRef();
    }
    
    UniformBuffer* Material::GetUniforms()
    {
        return m_Uniforms->GetUniforms();
    }
    
    
    void Material::SetRoughness(f32 Roughness) { m_Uniforms->SetRoughness(Roughness); m_HasUpdated = true; }
    f32 Material::GetRoughness() const { return m_Uniforms->GetRoughness(); }
    void Material::SetMetalness(f32 Metalness) { m_Uniforms->SetMetalness(Metalness); m_HasUpdated = true; }
    f32 Material::GetMetalness() const { return m_Uniforms->GetMetalness(); }
    void Material::SetShininess(f32 Shininess) { m_Uniforms->SetShininess(Shininess); m_HasUpdated = true; }
    f32 Material::GetShininess() const { return m_Uniforms->GetShininess(); }
    
    void   Material::SetDiffuse (const Vec4& Color) { m_Uniforms->SetDiffuse(Color); m_HasUpdated = true; }
    Vec4   Material::GetDiffuse () const            { return m_Uniforms->GetDiffuse(); }
    void   Material::SetSpecular(Scalar Factor)     { m_Uniforms->SetSpecular(Factor); m_HasUpdated = true; }
    Scalar Material::GetSpecular() const            { return m_Uniforms->GetSpecular(); }
    void   Material::SetEmissive(const Vec4& Color) { m_Uniforms->SetEmissive(Color); m_HasUpdated = true; }
    Vec4   Material::GetEmissive() const            { return m_Uniforms->GetEmissive(); }
    
    void Material::SetMinParallaxLayers(f32 Min  ) { m_Uniforms->SetMinParallaxLayers(Min); m_HasUpdated = true; }
    void Material::SetMaxParallaxLayers(f32 Max  ) { m_Uniforms->SetMaxParallaxLayers(Max); m_HasUpdated = true; }
    void Material::SetParallaxScale    (f32 Scale) { m_Uniforms->SetParallaxScale  (Scale); m_HasUpdated = true; }
    f32 Material::GetMinParallaxLayers() const { return m_Uniforms->GetMinParallaxLayers(); }
    f32 Material::GetMaxParallaxLayers() const { return m_Uniforms->GetMaxParallaxLayers(); }
    f32 Material::GetParallaxScale    () const { return m_Uniforms->GetParallaxScale    (); }
    
    void Material::UpdateUniforms()
    {
        if(!m_HasUpdated) return;
        m_HasUpdated = false;
        m_Uniforms->UpdateUniforms();
    }
};
