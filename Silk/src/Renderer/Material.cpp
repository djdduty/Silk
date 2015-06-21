#include <Renderer/Material.h>

namespace Silk
{
    static string MaterialMapNames[Material::MT_COUNT] =
    {
        "u_PositionMap"    ,
        "u_NormalMap"      ,
        "u_DiffuseMap"     ,
        "u_SpecularMap"    ,
        "u_EmissiveMap"    ,
        "u_DisplacementMap",
        "u_RoughnessMap"   ,
        "u_MetalnessMap"   ,
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
    
    Material::Material()
    {
        for(i32 i = 0;i < MT_COUNT;i++) m_Maps[i] = 0;
    }
};
