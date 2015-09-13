#include <Renderer/Renderer.h>
#include <Utilities/Utilities.h>
#include <Raster/Raster.h>

#include <Renderer/DefaultShaderCode.h>

namespace Silk
{
    static const string BlockNames[ShaderGenerator::IUT_COUNT] =
    {
        "MaterialData",
        "EngineData",
        "RenderData",
        "ObjectData",
        "UserData",
    };
    string GetUniformBlockTypeName(ShaderGenerator::INPUT_UNIFORM_TYPE Type)
    {
        return BlockNames[Type];
    }
    
    ShaderGenerator::ShaderGenerator(Renderer* r) : m_Renderer(r), m_ShadersGenerated(0)
    {
        Reset();
        m_NullModelUniforms = new ModelUniformSet(r,0);
        m_NullMaterialUniforms = new MaterialUniformSet(r);
        m_AllowInstancing = false;
        m_AllowInstancedTextureMatrix = r->GetRasterizer()->SupportsInstanceTextureTransforms();
        m_LightingMode = LM_FLAT;
        m_ParallaxFunction = PF_OCCLUSION;
		m_UseParallaxShadows = false;
    }
    ShaderGenerator::~ShaderGenerator()
    {
        delete m_NullModelUniforms;
        delete m_NullMaterialUniforms;
    }
    
    void ShaderGenerator::Reset()
    {
        m_ShaderVersion    = 330;
        m_LightingMode     = LM_FLAT;
        m_UserUniforms     = 0;
        m_AllowInstancing  = false;
        for(i32 i = 0;i < Material::MT_COUNT;i++) m_MapTypesUsed [i] = false;
        for(i32 i = 0;i < IAT_COUNT;i++) m_AttributeInputsUsed   [i] = false;
        for(i32 i = 0;i < IAT_COUNT;i++) m_AttributeOutputsUsed  [i] = false;
        for(i32 i = 0;i < IUT_COUNT;i++) m_UniformInputsUsed     [i] = false;
        for(i32 i = 0;i < OFT_COUNT;i++) m_FragmentOutputsUsed   [i] = false;
        
        m_VertexBlocks  .clear();
        m_GeometryBlocks.clear();
        m_FragmentBlocks.clear();
    }
    
    void ShaderGenerator::AddVertexModule(CString Code,i32 Index)
    {
        if(ReadBlocks(Code,Index,0) < 0) ERROR("Invalid shader module, no code blocks found.\n");
    }
    void ShaderGenerator::AddGeometryModule(CString Code,i32 Index)
    {
        if(ReadBlocks(Code,Index,1) < 0) ERROR("Invalid shader module, no code blocks found.\n");
    }
    void ShaderGenerator::AddFragmentModule(CString Code,i32 Index)
    {
        if(ReadBlocks(Code,Index,2) < 0) ERROR("Invalid shader module, no code blocks found.\n");
    }
    void ShaderGenerator::SetAllowInstancing(bool Flag)
    {
        m_AllowInstancing = Flag;
        m_AttributeInputsUsed[IAT_INSTANCE_TRANSFORM        ] =
        m_AttributeInputsUsed[IAT_INSTANCE_NORMAL_TRANSFORM ] =
        m_AttributeInputsUsed[IAT_INSTANCE_TEXTURE_TRANSFORM] = Flag;
    }
    Shader* ShaderGenerator::Generate()
    {
        string VertexShader   = GenerateVertexShader  ();
        string GeometryShader = GenerateGeometryShader();
        string FragmentShader = GenerateFragmentShader();
        
        Shader* S = m_Renderer->GetRasterizer()->CreateShader();
        if(!S->Load(const_cast<CString>(VertexShader.c_str()),0,const_cast<CString>(FragmentShader.c_str())))
        {
            m_Renderer->GetRasterizer()->Destroy(S);
            S = 0;
            printf("Vertex:\n%s\n\nFragment:\n%s\n",VertexShader.c_str(),FragmentShader.c_str());
        }
        else
        {
            for(i32 i = 0;i < OFT_COUNT;i++)
            {
                S->m_FragmentOutputs[i] = m_FragmentOutputsUsed[i];
                m_Renderer->RequireFragmentOutput((OUTPUT_FRAGMENT_TYPE)i);
            }
            for(i32 i = 0;i < IUT_COUNT;i++) S->m_UniformInputs  [i] = m_UniformInputsUsed  [i];
            S->m_ID = m_ShadersGenerated;
            S->m_SupportsInstancing = m_AllowInstancing;
            m_ShadersGenerated++;
        }

        return S;
    }
    string ShaderGenerator::GenerateVertexShader()
    {
        CodeBlock* SetGLPositionI  = 0;
        CodeBlock* SetPositionI    = 0;
        CodeBlock* SetNormalI      = 0;
        CodeBlock* SetTangentI     = 0;
        CodeBlock* SetTexCI        = 0;
        CodeBlock* SetGLPositionNI = 0;
        CodeBlock* SetPositionNI   = 0;
        CodeBlock* SetNormalNI     = 0;
        CodeBlock* SetTangentNI    = 0;
        CodeBlock* SetTexCNI       = 0;
        CodeBlock* SetColor        = 0;
        CodeBlock* SetRoughness    = 0;
        CodeBlock* SetMetalness    = 0;
        
        vector<i32> UnsortedBlockIndices;
        
        for(i32 i = 0;i < m_VertexBlocks.size();i++)
        {
            if     (m_VertexBlocks[i].ID == "SetGLPosition"         ) SetGLPositionNI = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetPosition"           ) SetPositionNI   = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetNormal"             ) SetNormalNI     = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTangent"            ) SetTangentNI    = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTexCoords"          ) SetTexCNI       = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetGLPositionInstanced") SetGLPositionI  = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetPositionInstanced"  ) SetPositionI    = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetNormalInstanced"    ) SetNormalI      = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTangentInstanced"   ) SetTangentI     = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTexCoordsInstanced" ) SetTexCI        = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetColor"              ) SetColor        = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetRoughness"          ) SetRoughness    = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetMetalness"          ) SetMetalness    = &m_VertexBlocks[i];
            
            UnsortedBlockIndices.push_back(i);
        }
        i32 CustomLightingBlock = -1;
        for(i32 i = 0;i < m_FragmentBlocks.size();i++)
        {
            if(m_FragmentBlocks[i].ID == "Lighting") { CustomLightingBlock = i; break; }
        }
        
        if(CustomLightingBlock == -1 && !SetColor)
        {
            if(m_LightingMode == LM_FLAT) SetAttributeInput(IAT_COLOR,true);
        }

        string VertexShader;
        VertexShader += FormatString("#version %d\n",m_ShaderVersion);
        
        /*
         * Attribute inputs
         */
        VertexShader += "\n";
        if(m_AttributeInputsUsed[IAT_POSITION                  ]) VertexShader += string("in vec3 " ) + PositionAttribName                 + ";\n";
        if(m_AttributeInputsUsed[IAT_NORMAL                    ]) VertexShader += string("in vec3 " ) + NormalAttribName                   + ";\n";
        if(m_AttributeInputsUsed[IAT_TANGENT                   ]) VertexShader += string("in vec3 " ) + TangentAttribName                  + ";\n";
        if(m_AttributeInputsUsed[IAT_COLOR                     ]) VertexShader += string("in vec4 " ) + ColorAttribName                    + ";\n";
        if(m_AttributeInputsUsed[IAT_TEXCOORD                  ]) VertexShader += string("in vec2 " ) + TexCoordAttribName                 + ";\n";
        if(m_AttributeInputsUsed[IAT_BONE_IDX                  ]) VertexShader += string("in ivec4 ") + BoneIndexAttribName                + ";\n";
        if(m_AttributeInputsUsed[IAT_BONE_WEIGHT               ]) VertexShader += string("in vec4 " ) + BoneWeightAttribName               + ";\n";
        if(m_AttributeInputsUsed[IAT_INSTANCE_TRANSFORM        ]) VertexShader += string("in mat4 " ) + InstanceTransformAttribName        + ";\n";
        if(m_AttributeInputsUsed[IAT_INSTANCE_NORMAL_TRANSFORM ]) VertexShader += string("in mat4 " ) + InstanceNormalTransformAttribName  + ";\n";
        if(m_AllowInstancedTextureMatrix) if(m_AttributeInputsUsed[IAT_INSTANCE_TEXTURE_TRANSFORM]) VertexShader += string("in mat4 " ) + InstanceTextureTransformAttribName + ";\n";

        /* Uniform dependencies */
        if(!m_UniformInputsUsed[IUT_OBJECT_UNIFORMS]) m_UniformInputsUsed[IUT_OBJECT_UNIFORMS] = m_AllowInstancing;
        if(m_MapTypesUsed[Material::MT_NORMAL]) m_UniformInputsUsed[IUT_OBJECT_UNIFORMS] = true;
        
        /*
         * Uniform inputs
         */
        for(i32 i = 0;i < IUT_COUNT;i++)
        {
            if(m_UniformInputsUsed[i])
            {
                VertexShader += "\n";
                VertexShader += GenerateInputBlock((INPUT_UNIFORM_TYPE)i);
            }
        }
        
        /*
         * Outputs
         */
        VertexShader += "\n";
        if(m_AttributeOutputsUsed[IAT_POSITION   ]) VertexShader += string("out vec3 ") + PositionOutName   + ";\n";
        if(m_AttributeOutputsUsed[IAT_NORMAL     ]) VertexShader += string("out vec3 ") + NormalOutName     + ";\n";
        if(m_AttributeOutputsUsed[IAT_TANGENT    ]) VertexShader += string("out vec3 ") + TangentOutName    + ";\n";
        if(m_AttributeOutputsUsed[IAT_COLOR      ]) VertexShader += string("out vec4 ") + ColorOutName      + ";\n";
        if(m_AttributeOutputsUsed[IAT_TEXCOORD   ]) VertexShader += string("out vec2 ") + TexCoordOutName   + ";\n";
        if(m_MapTypesUsed[Material::MT_NORMAL    ]) VertexShader += string("out mat3 TBN;\n");
        
        /*
         * main
         */
        VertexShader += "\n";
        VertexShader += "void main()\n{\n";
        
        if(m_AllowInstancing)
        {
            bool nSetTexCI = m_AllowInstancedTextureMatrix && !SetTexCI;
            VertexShader += IsInstancedConditional_0;
            
            if(!SetGLPositionI ) VertexShader += DefaultGLPositionInstancedFunc ;
            if(!SetPositionI   && m_AttributeOutputsUsed[IAT_POSITION]) VertexShader += DefaultPositionInstancedFunc   ;
            if(!SetNormalI     && m_AttributeOutputsUsed[IAT_NORMAL  ]) VertexShader += DefaultNormalInstancedFunc     ;
            if(!SetTangentI    && m_AttributeOutputsUsed[IAT_TANGENT ]) VertexShader += DefaultTangentInstancedFunc    ;
            if(nSetTexCI       && m_AttributeOutputsUsed[IAT_TEXCOORD]) VertexShader += DefaultTexCoordInstancedFunc   ;
            else if(!SetTexCI  && m_AttributeOutputsUsed[IAT_TEXCOORD]) VertexShader += DefaultTexCoordNonInstancedFunc;
            
            VertexShader += IsInstancedConditional_1;
            
            if(!SetGLPositionNI) VertexShader += DefaultGLPositionNonInstancedFunc;
            if(!SetPositionNI  && m_AttributeOutputsUsed[IAT_POSITION]) VertexShader += DefaultPositionNonInstancedFunc  ;
            if(!SetNormalNI    && m_AttributeOutputsUsed[IAT_NORMAL  ]) VertexShader += DefaultNormalNonInstancedFunc    ;
            if(!SetTangentNI   && m_AttributeOutputsUsed[IAT_TANGENT ]) VertexShader += DefaultTangentNonInstancedFunc   ;
            if(!SetTexCNI      && m_AttributeOutputsUsed[IAT_TEXCOORD]) VertexShader += DefaultTexCoordNonInstancedFunc  ;
            
            VertexShader += IsInstancedConditional_2;
        }
        else
        {
            if(!SetGLPositionNI) VertexShader += DefaultGLPositionNonInstancedFunc;
            if(!SetPositionNI  && m_AttributeOutputsUsed[IAT_POSITION]) VertexShader += DefaultPositionNonInstancedFunc  ;
            if(!SetNormalNI    && m_AttributeOutputsUsed[IAT_NORMAL  ]) VertexShader += DefaultNormalNonInstancedFunc    ;
            if(!SetTangentNI   && m_AttributeOutputsUsed[IAT_TANGENT ]) VertexShader += DefaultTangentNonInstancedFunc   ;
            if(!SetTexCNI      && m_AttributeOutputsUsed[IAT_TEXCOORD]) VertexShader += DefaultTexCoordNonInstancedFunc  ;
        }
        
        if(!SetColor     && m_AttributeInputsUsed[IAT_COLOR] && m_AttributeOutputsUsed[IAT_COLOR]) VertexShader += DefaultColorFunc;
        //if(!SetRoughness && m_UniformInputsUsed  [IUT_MATERIAL_UNIFORMS]) VertexShader += DefaultRoughnessFunc;
        //if(!SetMetalness && m_UniformInputsUsed  [IUT_MATERIAL_UNIFORMS]) VertexShader += DefaultMetalnessFunc;
        
        if(m_MapTypesUsed[Material::MT_NORMAL])
        {
            VertexShader += string("\t\tTBN = mat3(") + TangentOutName + ",normalize(cross(" + NormalOutName + "," + TangentOutName + "))," + NormalOutName + ");\n";
        }
        
        /*
         *
         *   Don't forget the default animation stuff
         *
         *
         */
        
        while(UnsortedBlockIndices.size() != 0)
        {
            i32 MinExecutionIndexIndex = UnsortedBlockIndices[0];
            i32 BlockIndexIndex = 0;
            for(i32 i = 0;i < UnsortedBlockIndices.size();i++)
            {
                if(m_VertexBlocks[UnsortedBlockIndices[i]].Index < m_VertexBlocks[MinExecutionIndexIndex].Index)
                {
                    MinExecutionIndexIndex = UnsortedBlockIndices[i];
                    BlockIndexIndex = i;
                }
            }
            UnsortedBlockIndices.erase(UnsortedBlockIndices.begin() + BlockIndexIndex);
            VertexShader += "\t" + m_VertexBlocks[MinExecutionIndexIndex].Code + "\n";
        }
        
        VertexShader += "}\n";
        return VertexShader;
    }
    string ShaderGenerator::GenerateGeometryShader()
    {
        return "";
    }
    string ShaderGenerator::GenerateFragmentShader()
    {
        string FragmentShader;
        FragmentShader += FormatString("#version %d\n",m_ShaderVersion);
        
        /*
         * Attribute inputs
         */
        FragmentShader += "\n";
        if(m_AttributeOutputsUsed[IAT_POSITION]) FragmentShader += string("in vec3 ") + PositionOutName   + ";\n";
        if(m_AttributeOutputsUsed[IAT_NORMAL  ]) FragmentShader += string("in vec3 ") + NormalOutName     + ";\n";
        if(m_AttributeOutputsUsed[IAT_TANGENT ]) FragmentShader += string("in vec3 ") + TangentOutName    + ";\n";
        if(m_AttributeOutputsUsed[IAT_COLOR   ]) FragmentShader += string("in vec4 ") + ColorOutName      + ";\n";
        if(m_AttributeOutputsUsed[IAT_TEXCOORD]) FragmentShader += string("in vec2 ") + TexCoordOutName   + ";\n";
        if(m_MapTypesUsed[Material::MT_NORMAL ]) FragmentShader += string("in mat3 TBN;\n");
        
        i32 CustomLightingBlock = -1;
        vector<i32> UnsortedBlockIndices;
        
        CodeBlock* PointLight       = 0;
        CodeBlock* SpotLight        = 0;
        CodeBlock* DirectionalLight = 0;
        CodeBlock* SetPosition      = 0;
        CodeBlock* SetNormal        = 0;
        CodeBlock* SetTangent       = 0;
        CodeBlock* SetColor         = 0;
        CodeBlock* SetTexC          = 0;
        CodeBlock* SetMaterial0     = 0;
        CodeBlock* SetMaterial1     = 0;
        CodeBlock* SetSpecular      = 0;
        CodeBlock* SetEmissive      = 0;
        
        for(i32 i = 0;i < m_FragmentBlocks.size();i++)
        {
            if     (m_FragmentBlocks[i].ID == "Lighting"        ) CustomLightingBlock = i;
            else if(m_FragmentBlocks[i].ID == "PointLight"      ) PointLight          = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SpotLight"       ) SpotLight           = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "DirectionalLight") DirectionalLight    = &m_FragmentBlocks[i];
            else UnsortedBlockIndices.push_back(i);
            
            if     (m_FragmentBlocks[i].ID == "SetPosition"     ) SetPosition         = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetNormal"       ) SetNormal           = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetTangent"      ) SetTangent          = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetColor"        ) SetColor            = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetTexCoords"    ) SetTexC             = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetMaterial0"    ) SetMaterial0        = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetMaterial1"    ) SetMaterial1        = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetSpecular"     ) SetSpecular         = &m_FragmentBlocks[i];
            else if(m_FragmentBlocks[i].ID == "SetEmissive"     ) SetEmissive         = &m_FragmentBlocks[i];
        }
        if((CustomLightingBlock == -1 && m_LightingMode == LM_PHONG) || m_MapTypesUsed[Material::MT_NORMAL]) SetUniformInput(IUT_RENDERER_UNIFORMS,true);
        if(m_MapTypesUsed[Material::MT_PARALLAX]) SetUniformInput(IUT_MATERIAL_UNIFORMS,true);
        
        /*
         * Uniform inputs
         */
        for(i32 i = 0;i < IUT_COUNT;i++)
        {
            if(m_UniformInputsUsed[i])
            {
                FragmentShader += "\n";
                FragmentShader += GenerateInputBlock((INPUT_UNIFORM_TYPE)i);
            }
        }
        
        /*
         * Sampler inputs
         */
         FragmentShader += "\n";
         for(i32 i = 0;i < Material::MT_COUNT;i++)
         {
             if(m_MapTypesUsed[i])
             {
                 FragmentShader += "uniform sampler2D " + GetShaderMapName((Material::MAP_TYPE)i) + ";\n";
             }
         }
        
        /*
         * Fragment outputs
         */
        FragmentShader += "\n";
        if(m_FragmentOutputsUsed[OFT_POSITION ]) FragmentShader += string("out vec4 ") + FragmentPositionOutputName  + ";\n";
        if(m_FragmentOutputsUsed[OFT_NORMAL   ]) FragmentShader += string("out vec4 ") + FragmentNormalOutputName    + ";\n";
        if(m_FragmentOutputsUsed[OFT_TANGENT  ]) FragmentShader += string("out vec4 ") + FragmentTangentOutputName   + ";\n";
        if(m_FragmentOutputsUsed[OFT_COLOR    ]) FragmentShader += string("out vec4 ") + FragmentColorOutputName     + ";\n";
        if(m_FragmentOutputsUsed[OFT_MATERIAL0]) FragmentShader += string("out vec4 ") + FragmentMaterial0OutputName + ";\n";
        if(m_FragmentOutputsUsed[OFT_MATERIAL1]) FragmentShader += string("out vec4 ") + FragmentMaterial1OutputName + ";\n";
        
        /*
         * Fragment calculation functions
         */
        if(m_MapTypesUsed[Material::MT_PARALLAX])
        {
            switch(m_ParallaxFunction)
            {
                case PF_RELIEF   : { FragmentShader += ReliefParallaxMappingFunction; break; }
                case PF_OCCLUSION: { FragmentShader += OcclusionParallaxFunction    ; break; }
                default          : { ERROR("Invalid parallax function.\n")          ; break; }
            }
			if(m_UseParallaxShadows) FragmentShader += ParallaxMappingShadowMultiplier;
        }
        
        /*
         * main
         */
        FragmentShader += "\n";
        FragmentShader += "void main()\n{\n";
        
        if(!SetPosition && m_AttributeOutputsUsed[IAT_POSITION]) FragmentShader += string("\tvec3 sPosition = ") + PositionOutName + ";\n";
        if(!SetTexC     && m_AttributeOutputsUsed[IAT_TEXCOORD])
        {
            if(m_MapTypesUsed[Material::MT_PARALLAX])
            {
                FragmentShader += "\tfloat Ht;\n";
                FragmentShader += string("\tvec2 sTexCoord = ParallaxOffset(normalize(u_CameraPosition - sPosition) * TBN,-") + NormalOutName + "," + TexCoordOutName + ",u_ParallaxScale,Ht);\n";
            }
            else FragmentShader += string("\tvec2 sTexCoord = ") + TexCoordOutName + ";\n";
        }
        
        if(!SetTangent  && m_AttributeOutputsUsed[IAT_TANGENT]) FragmentShader += string("\tvec3 sTangent  = normalize(") + TangentOutName  + ");\n";
        if(!SetNormal   && m_AttributeOutputsUsed[IAT_NORMAL])
        {
            if(!m_MapTypesUsed[Material::MT_NORMAL] && m_AttributeOutputsUsed[IAT_NORMAL ]) FragmentShader += string("\tvec3 sNormal = normalize(") + NormalOutName + ");\n";
            else if(m_MapTypesUsed[Material::MT_NORMAL])
            {
                FragmentShader += string("\tvec3 sNormal = TBN * normalize(texture(") + GetShaderMapName(Material::MT_NORMAL) + ",sTexCoord).rgb * 2.0 - 1.0);\n";
            }
        }
        if(!SetColor    )
        {
            if(!m_MapTypesUsed[Material::MT_DIFFUSE] && m_AttributeOutputsUsed[IAT_COLOR]) FragmentShader += string("\tvec4 sColor = ") + ColorOutName + ";\n";
            else if(m_MapTypesUsed[Material::MT_DIFFUSE]) FragmentShader += string("\tvec4 sColor = texture(") + GetShaderMapName(Material::MT_DIFFUSE) + ",sTexCoord);\n";
        }
        if(!SetSpecular )
        {
            if(!m_MapTypesUsed[Material::MT_SPECULAR] && m_UniformInputsUsed[IUT_MATERIAL_UNIFORMS]) FragmentShader += string("\tvec4 sSpecular = u_Specular;\n");
            else if(m_MapTypesUsed[Material::MT_SPECULAR]) FragmentShader += string("\tvec4 sSpecular = ") + GetShaderMapName(Material::MT_SPECULAR) + ",sTexCoord);\n";
        }
        if(!SetEmissive )
        {
            if(!m_MapTypesUsed[Material::MT_EMISSIVE] && m_UniformInputsUsed[IUT_MATERIAL_UNIFORMS]) FragmentShader += string("\tvec4 sEmissive = u_Emissive;\n");
            else if(m_MapTypesUsed[Material::MT_EMISSIVE]) FragmentShader += string("\tvec4 sEmissive = ") + GetShaderMapName(Material::MT_EMISSIVE) + ",sTexCoord);\n";
        }
        
        //Two custom material maps, not sure if this is useful or not yet
        if(!SetMaterial0 && m_MapTypesUsed[Material::MT_MATERIAL0]) //Only use Material0 if a map is going to be set
        {
            FragmentShader += string("\tvec4 sMaterial0 = texture(") + GetShaderMapName(Material::MT_MATERIAL0) + ",sTexCoord);\n";
        }
        if(!SetMaterial1 && m_MapTypesUsed[Material::MT_MATERIAL1]) //Only use Material1 if a map is going to be set
        {
            FragmentShader += string("\tvec4 sMaterial1 = texture(") + GetShaderMapName(Material::MT_MATERIAL1) + ",sTexCoord);\n";
        }
        
        while(UnsortedBlockIndices.size() != 0)
        {
            i32 MinExecutionIndexIndex = UnsortedBlockIndices[0];
            i32 BlockIndexIndex = 0;
            for(i32 i = 0;i < UnsortedBlockIndices.size();i++)
            {
                if(m_FragmentBlocks[UnsortedBlockIndices[i]].Index < m_FragmentBlocks[MinExecutionIndexIndex].Index)
                {
                    MinExecutionIndexIndex = UnsortedBlockIndices[i];
                    BlockIndexIndex = i;
                }
            }
            UnsortedBlockIndices.erase(UnsortedBlockIndices.begin() + BlockIndexIndex);
            FragmentShader += "\t" + m_FragmentBlocks[MinExecutionIndexIndex].Code + "\n";
        }
        
        
        if(CustomLightingBlock != -1) FragmentShader += m_FragmentBlocks[CustomLightingBlock].Code;
        else
        {
            switch(m_LightingMode)
            {
                case LM_PHONG:
                {
                    FragmentShader += DefaultFragmentShaderBase_0;
                    
                    if(!PointLight) FragmentShader += DefaultFragmentShaderPointLight;
                    else FragmentShader += PointLight->Code;
                    
					if(m_MapTypesUsed[Material::MT_PARALLAX] && m_UseParallaxShadows)
                    {
                        FragmentShader += string("\t\t\t\tf_Color *= pow(ParallaxSoftShadowMultiplier(Dir,") + TexCoordOutName + ",u_ParallaxScale,Ht),4);\n";
                    }
                    
                    FragmentShader += DefaultFragmentShaderBase_1;
                    
                    if(!SpotLight) FragmentShader += DefaultFragmentShaderSpotLight;
                    else FragmentShader += SpotLight->Code;
                    
                    if(m_MapTypesUsed[Material::MT_PARALLAX] && m_UseParallaxShadows)
                    {
                        FragmentShader += string("\t\t\t\tf_Color *= pow(ParallaxSoftShadowMultiplier(Dir,") + TexCoordOutName + ",u_ParallaxScale,Ht),4);\n";
                    }
                    
                    FragmentShader += DefaultFragmentShaderBase_2;
                    
                    if(!DirectionalLight) FragmentShader += DefaultFragmentShaderDirectionalLight;
                    else FragmentShader += DirectionalLight->Code;
                    
                    if(m_MapTypesUsed[Material::MT_PARALLAX] && m_UseParallaxShadows)
                    {
                        FragmentShader += string("\t\t\t\tf_Color *= pow(ParallaxSoftShadowMultiplier(u_Lights[l].Direction.xyz,") + TexCoordOutName + ",u_ParallaxScale,Ht),4);\n";
                    }
                    
                    FragmentShader += DefaultFragmentShaderBase_3;
                    break;
                }
                case LM_FLAT:
                {
                    //Done by default code now, but I'll leave it here just in case something changes
                    //FragmentShader += DefaultFlatFragmentShader;
                    break;
                }
            }
        }
        
        if(m_FragmentOutputsUsed[OFT_NORMAL   ]) FragmentShader += string(FragmentNormalOutputName   ) + " = sNormal   ;\n";
        if(m_FragmentOutputsUsed[OFT_TANGENT  ]) FragmentShader += string(FragmentTangentOutputName  ) + " = sTangent  ;\n";
        if(m_FragmentOutputsUsed[OFT_POSITION ]) FragmentShader += string(FragmentPositionOutputName ) + " = sPosition ;\n";
        if(m_FragmentOutputsUsed[OFT_MATERIAL0]) FragmentShader += string(FragmentMaterial0OutputName) + " = sMaterial0;\n";
        if(m_FragmentOutputsUsed[OFT_MATERIAL1]) FragmentShader += string(FragmentMaterial1OutputName) + " = sMaterial1;\n";
        if(m_FragmentOutputsUsed[OFT_COLOR    ] && m_LightingMode == LM_FLAT) FragmentShader += string("\t") + FragmentColorOutputName + " = sColor;\n";
        
        FragmentShader += "}\n";
        
        return FragmentShader;
    }
    i32 ShaderGenerator::ReadBlocks(CString Code,i32 Index,i32 ShaderType)
    {
        i32 p = 0;
        while(Code[p] != 0)
        {
            if(Code[p] == '[')
            {
                i32 r = ParseBlock(Code,p + 1,Index,ShaderType);
                if(r == -1) return -1;
                
                p = r;
            }
            p++;
        }
        return 0;
    }
    i32 ShaderGenerator::ParseBlock(CString Code,i32 StartIndex,i32 ExecutionIndex,i32 ShaderType)
    {
        i32 p = StartIndex;
        
        CodeBlock b;
        while(Code[p] != 0 && Code[p] != ']') { b.ID += Code[p]; p++; }
        if(Code[p] ==  0) { ERROR("Unexpected end of shader template file.\n"); return -1; }
        p++;
        string BlockCode;
        while(Code[p] != 0 && !(Code[p] == '[' && Code[p + 1] == '/')) { b.Code += Code[p]; p++; }
        if(Code[p] ==  0) { ERROR("Unexpected end of shader template file.\n"); return -1; }
        
        b.Index = ExecutionIndex;
        
        if     (ShaderType == 0) m_VertexBlocks  .push_back(b);
        else if(ShaderType == 1) m_GeometryBlocks.push_back(b);
        else if(ShaderType == 2) m_FragmentBlocks.push_back(b);
        return p;
    }
    string ShaderGenerator::GenerateInputBlock(INPUT_UNIFORM_TYPE Type)
    {
        UniformBuffer* eUniforms = 0;
        switch(Type)
        {
            case IUT_MATERIAL_UNIFORMS: { eUniforms = m_NullMaterialUniforms->GetUniforms() ; break; }
            case IUT_ENGINE_UNIFORMS  : { eUniforms = m_Renderer->GetEngineUniformBuffer  (); break; }
            case IUT_RENDERER_UNIFORMS: { eUniforms = m_Renderer->GetRendererUniformBuffer(); break; }
            case IUT_OBJECT_UNIFORMS  : { eUniforms = m_NullModelUniforms->GetUniforms()    ; break; }
            case IUT_USER_UNIFORMS    : { eUniforms = m_UserUniforms                        ; break; }
            default:
            {
                ERROR("Invalid input type, ignoring.\n");
            }
        }
        
        if(!eUniforms) return "";
        
        string Code;
        if(Type == ShaderGenerator::IUT_OBJECT_UNIFORMS) Code += LightDataStructure + "\n";
        //Code += "\nlayout (std140) uniform " + GetUniformBlockTypeName(Type) + "\n{\n";
        
        i32 uCount = eUniforms->GetUniformCount();
        for(i32 i = 0;i < uCount;i++)
        {
            #define UCase(TypeString,Enum) case Enum: { Code += TypeString; break; }
            switch(eUniforms->GetUniformInfo(i)->Type)
            {
                UCase("uniform bool "  ,UT_BOOL  );
                UCase("uniform int "   ,UT_INT   );
                UCase("uniform uint "  ,UT_UINT  );
                UCase("uniform float " ,UT_FLOAT );
                UCase("uniform double ",UT_DOUBLE);
                UCase("uniform vec2 "  ,UT_VEC2  );
                UCase("uniform vec3 "  ,UT_VEC3  );
                UCase("uniform vec4 "  ,UT_VEC4  );
                UCase("uniform mat4 "  ,UT_MAT4  );
                UCase("uniform Light " ,UT_LIGHT );
            }
            Code += eUniforms->GetUniformInfo(i)->Name;
            if(eUniforms->GetUniformInfo(i)->ArraySize != -1) Code += FormatString("[%d];\n",eUniforms->GetUniformInfo(i)->ArraySize);
            else Code += ";\n";
        }
        //Code += "};\n";
        
        return Code;
    }
};
