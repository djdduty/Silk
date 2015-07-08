#include <Renderer/Material.h>
#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>

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
    };
    string GetShaderMapName(Material::MAP_TYPE Type)
    {
        return MaterialMapNames[Type];
    }
    
    Material::Material(Renderer* r) : m_Renderer(r)
    {
        for(i32 i = 0;i < MT_COUNT;i++) m_Maps[i] = 0;
        m_Uniforms = new MaterialUniformSet(m_Renderer);
    }
    Material::~Material()
    {
        delete m_Uniforms;
    }
    
    UniformBuffer* Material::GetUniforms()
    {
        return m_Uniforms->GetUniforms();
    }
    
    
    void Material::SetRoughness(f32 Roughness) { m_Uniforms->SetRoughness(Roughness); }
    f32 Material::GetRoughness() const { return m_Uniforms->GetRoughness(); }
    void Material::SetMetalness(f32 Metalness) { m_Uniforms->SetMetalness(Metalness); }
    f32 Material::GetMetalness() const { return m_Uniforms->GetMetalness(); }
    void Material::SetShininess(f32 Shininess) { m_Uniforms->SetShininess(Shininess); }
    f32 Material::GetShininess() const { return m_Uniforms->GetShininess(); }
    
    void Material::SetDiffuse (const Vec4& Color) { m_Uniforms->SetDiffuse(Color); }
    Vec4 Material::GetDiffuse() const { return m_Uniforms->GetDiffuse(); }
    void Material::SetSpecular(const Vec4& Color) { m_Uniforms->SetSpecular(Color); }
    Vec4 Material::GetSpecular() const { return m_Uniforms->GetSpecular(); }
    void Material::SetEmissive(const Vec4& Color) { m_Uniforms->SetEmissive(Color); }
    Vec4 Material::GetEmissive() const { return m_Uniforms->GetEmissive(); }
    
    void Material::SetMinParallaxLayers(f32 Min  ) { m_Uniforms->SetMinParallaxLayers(Min); }
    void Material::SetMaxParallaxLayers(f32 Max  ) { m_Uniforms->SetMaxParallaxLayers(Max); }
    void Material::SetParallaxScale    (f32 Scale) { m_Uniforms->SetParallaxScale  (Scale); }
    f32 Material::GetMinParallaxLayers() const { return m_Uniforms->GetMinParallaxLayers(); }
    f32 Material::GetMaxParallaxLayers() const { return m_Uniforms->GetMaxParallaxLayers(); }
    f32 Material::GetParallaxScale    () const { return m_Uniforms->GetParallaxScale    (); }
    
    void Material::UpdateUniforms()
    {
        m_Uniforms->UpdateUniforms();
    }
};
