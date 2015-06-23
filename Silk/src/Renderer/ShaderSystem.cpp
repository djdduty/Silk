#include <Renderer/ShaderSystem.h>
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
    
    ShaderGenerator::ShaderGenerator(Renderer* r) : m_Renderer(r)
    {
        Reset();
    }
    ShaderGenerator::~ShaderGenerator()
    {
    }
    
    void ShaderGenerator::Reset()
    {
        m_ShaderVersion    = 330;
        m_LightingMode     = LM_FLAT;
        m_MaterialUniforms = 0;
        m_UserUniforms     = 0;
        for(i32 i = 0;i < Material::MT_COUNT;i++) m_MapTypesUsed[i] = false;
        for(i32 i = 0;i < IAT_COUNT;i++) m_AttributeInputsUsed  [i] = false;
        for(i32 i = 0;i < IUT_COUNT;i++) m_UniformInputsUsed    [i] = false;
        for(i32 i = 0;i < OFT_COUNT;i++) m_FragmentOutputsUsed  [i] = false;
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
    Shader* ShaderGenerator::Generate()
    {
        string VertexShader   = GenerateVertexShader  ();
        string GeometryShader = GenerateGeometryShader();
        string FragmentShader = GenerateFragmentShader();
        
        printf("Shader generated:\nVertex:\n%s\n\nFragment:\n%s\n",VertexShader.c_str(),FragmentShader.c_str());
        
        Shader* S = m_Renderer->GetRasterizer()->CreateShader();
        if(!S->Load(const_cast<CString>(VertexShader.c_str()),0,const_cast<CString>(FragmentShader.c_str())))
        {
            m_Renderer->GetRasterizer()->Destroy(S);
            S = 0;
        }
        else
        {
            for(i32 i = 0;i < OFT_COUNT;i++) S->m_FragmentOutputs[i] = m_FragmentOutputsUsed[i];
            for(i32 i = 0;i < IUT_COUNT;i++) S->m_UniformInputs  [i] = m_UniformInputsUsed  [i];
        }
        return S;
    }
    string ShaderGenerator::GenerateVertexShader()
    {
        CodeBlock* SetPosition  = 0;
        CodeBlock* SetNormal    = 0;
        CodeBlock* SetTangent   = 0;
        CodeBlock* SetColor     = 0;
        CodeBlock* SetTexC      = 0;
        CodeBlock* SetRoughness = 0;
        CodeBlock* SetMetalness = 0;
        
        vector<i32> UnsortedBlockIndices;
        
        for(i32 i = 0;i < m_VertexBlocks.size();i++)
        {
            if     (m_VertexBlocks[i].ID == "SetPosition"      ) SetPosition  = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetNormal"        ) SetNormal    = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTangent"       ) SetTangent   = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetColor"         ) SetColor     = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetTexCoords"     ) SetTexC      = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetRoughness"     ) SetRoughness = &m_VertexBlocks[i];
            else if(m_VertexBlocks[i].ID == "SetMetalness"     ) SetMetalness = &m_VertexBlocks[i];
            
            UnsortedBlockIndices.push_back(i);
        }
        i32 CustomLightingBlock = -1;
        for(i32 i = 0;i < m_FragmentBlocks.size();i++)
        {
            if(m_FragmentBlocks[i].ID == "Lighting") { CustomLightingBlock = i; break; }
        }
        
        if(CustomLightingBlock == -1)
        {
            if(m_LightingMode == LM_FLAT) SetAttributeInput(IAT_COLOR,true);
        }
        
        /* Default code dependencies */
        if(!SetPosition                       ) m_UniformInputsUsed[IUT_RENDERER_UNIFORMS] = true;
        
        if((!SetNormal  && m_AttributeInputsUsed[IAT_NORMAL  ])
        || (!SetTangent && m_AttributeInputsUsed[IAT_TANGENT ])
        || (!SetTexC    && m_AttributeInputsUsed[IAT_TEXCOORD])) m_UniformInputsUsed[IUT_OBJECT_UNIFORMS] = true;
        
        string VertexShader;
        VertexShader += FormatString("#version %d\n",m_ShaderVersion);
        /*
         * Attribute inputs
         */
        VertexShader += "\n";
        if(m_AttributeInputsUsed[IAT_POSITION   ]) VertexShader += string("in vec3 " ) + PositionAttribName   + ";\n";
        if(m_AttributeInputsUsed[IAT_NORMAL     ]) VertexShader += string("in vec3 " ) + NormalAttribName     + ";\n";
        if(m_AttributeInputsUsed[IAT_TANGENT    ]) VertexShader += string("in vec3 " ) + TangentAttribName    + ";\n";
        if(m_AttributeInputsUsed[IAT_COLOR      ]) VertexShader += string("in vec4 " ) + ColorAttribName      + ";\n";
        if(m_AttributeInputsUsed[IAT_TEXCOORD   ]) VertexShader += string("in vec2 " ) + TexCoordAttribName   + ";\n";
        if(m_AttributeInputsUsed[IAT_BONE_IDX   ]) VertexShader += string("in ivec4 ") + BoneIndexAttribName  + ";\n";
        if(m_AttributeInputsUsed[IAT_BONE_WEIGHT]) VertexShader += string("in vec4 " ) + BoneWeightAttribName + ";\n";
        
        /*
         * Uniform inputs
         */
        for(i32 i = 0;i < IUT_COUNT;i++)
        {
            if(m_UniformInputsUsed[i])
            {
                VertexShader += GenerateInputBlock((INPUT_UNIFORM_TYPE)i);
            }
        }
        
        /*
         * Outputs
         */
        VertexShader += "\n";
        if(m_AttributeInputsUsed[IAT_POSITION   ]) VertexShader += string("out vec3 ") + PositionOutName   + ";\n";
        if(m_AttributeInputsUsed[IAT_NORMAL     ]) VertexShader += string("out vec3 ") + NormalOutName     + ";\n";
        if(m_AttributeInputsUsed[IAT_TANGENT    ]) VertexShader += string("out vec3 ") + TangentOutName    + ";\n";
        if(m_AttributeInputsUsed[IAT_COLOR      ]) VertexShader += string("out vec4 ") + ColorOutName      + ";\n";
        if(m_AttributeInputsUsed[IAT_TEXCOORD   ]) VertexShader += string("out vec2 ") + TexCoordOutName   + ";\n";
        
        /*
         * main
         */
        VertexShader += "\n";
        VertexShader += "void main()\n{\n";
        
        if(!SetPosition  && m_AttributeInputsUsed[IAT_POSITION         ]) VertexShader += string("\tgl_Position = u_MVP * vec4(") + PositionAttribName + ",1.0);\n\t" +
                                                                                                         PositionOutName  + " = "                       + PositionAttribName  + ";\n";
        if(!SetNormal    && m_AttributeInputsUsed[IAT_NORMAL           ]) VertexShader += string("\t") + NormalOutName    + " = u_NormalMatrix * vec4(" + NormalAttribName    + ",1.0f);\n";
        if(!SetTangent   && m_AttributeInputsUsed[IAT_TANGENT          ]) VertexShader += string("\t") + TangentOutName   + " = u_NormalMatrix * vec4(" + TangentAttribName   + ",1.0f);\n";
        if(!SetColor     && m_AttributeInputsUsed[IAT_COLOR            ]) VertexShader += string("\t") + ColorOutName     + " = " + ColorAttribName + ";\n";
        if(!SetTexC      && m_AttributeInputsUsed[IAT_TEXCOORD         ]) VertexShader += string("\t") + TexCoordOutName  + " = u_TextureMatrix * vec3(" + TexCoordAttribName + ",1.0f);\n";
        if(!SetRoughness && m_UniformInputsUsed  [IUT_MATERIAL_UNIFORMS]) VertexShader += string("\t") + RoughnessOutName + " = u_Roughness;\n";
        if(!SetMetalness && m_UniformInputsUsed  [IUT_MATERIAL_UNIFORMS]) VertexShader += string("\t") + MetalnessOutName + " = u_Metalness;\n";
        
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
        if(m_AttributeInputsUsed[IAT_POSITION   ]) FragmentShader += string("in vec3 " ) + PositionOutName   + ";\n";
        if(m_AttributeInputsUsed[IAT_NORMAL     ]) FragmentShader += string("in vec3 " ) + NormalOutName     + ";\n";
        if(m_AttributeInputsUsed[IAT_TANGENT    ]) FragmentShader += string("in vec3 " ) + TangentOutName    + ";\n";
        if(m_AttributeInputsUsed[IAT_COLOR      ]) FragmentShader += string("in vec4 " ) + ColorOutName      + ";\n";
        if(m_AttributeInputsUsed[IAT_TEXCOORD   ]) FragmentShader += string("in vec2 " ) + TexCoordOutName   + ";\n";
        
        i32 CustomLightingBlock = -1;
        vector<i32> UnsortedBlockIndices;
        
        CodeBlock* PointLight       = 0;
        CodeBlock* SpotLight        = 0;
        CodeBlock* DirectionalLight = 0;
        CodeBlock* SetPosition  = 0;
        CodeBlock* SetNormal    = 0;
        CodeBlock* SetTangent   = 0;
        CodeBlock* SetColor     = 0;
        CodeBlock* SetTexC      = 0;
        CodeBlock* SetMaterial0 = 0;
        CodeBlock* SetMaterial1 = 0;
        
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
        }
        if(CustomLightingBlock == -1 && m_LightingMode == LM_PHONG) SetUniformInput(IUT_RENDERER_UNIFORMS,true);
        
        /*
         * Uniform inputs
         */
        for(i32 i = 0;i < IUT_COUNT;i++)
        {
            if(m_UniformInputsUsed[i])
            {
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
         * main
         */
        FragmentShader += "\n";
        FragmentShader += "void main()\n{\n";
        
        if(m_FragmentOutputsUsed[OFT_POSITION ] && !SetPosition)
        {
            if(!m_MapTypesUsed[Material::MT_POSITION]) FragmentShader += string("\t") + FragmentPositionOutputName  + " = " + PositionOutName  + ";\n";
            else FragmentShader += string("\t") + FragmentPositionOutputName + " = texture(" + GetShaderMapName(Material::MT_POSITION) + "," + TexCoordOutName + ");\n";
        }
        if(m_FragmentOutputsUsed[OFT_NORMAL   ] && !SetNormal  )
        {
            if(!m_MapTypesUsed[Material::MT_NORMAL  ]) FragmentShader += string("\t") + FragmentNormalOutputName    + " = " + NormalOutName    + ";\n";
            else FragmentShader += string("\t") + FragmentPositionOutputName + " = texture(" + GetShaderMapName(Material::MT_NORMAL  ) + "," + TexCoordOutName + ");\n";
        }
        if(m_FragmentOutputsUsed[OFT_TANGENT  ] && !SetTangent ) FragmentShader += string("\t") + FragmentTangentOutputName   + " = " + TangentOutName   + ";\n";
        if(m_FragmentOutputsUsed[OFT_COLOR    ] && !SetColor   )
        {
            if(!m_MapTypesUsed[Material::MT_DIFFUSE ]) FragmentShader += string("\t") + FragmentColorOutputName     + " = " + ColorOutName     + ";\n";
            else FragmentShader += string("\t") + FragmentColorOutputName + " = texture(" + GetShaderMapName(Material::MT_DIFFUSE  ) + "," + TexCoordOutName + ");\n";
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
                    
                    FragmentShader += DefaultFragmentShaderBase_1;
                    
                    if(!SpotLight) FragmentShader += DefaultFragmentShaderSpotLight;
                    else FragmentShader += SpotLight->Code;
                    
                    FragmentShader += DefaultFragmentShaderBase_2;
                    
                    if(!DirectionalLight) FragmentShader += DefaultFragmentShaderDirectionalLight;
                    else FragmentShader += DirectionalLight->Code;
                    
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
            case IUT_MATERIAL_UNIFORMS: { eUniforms = m_MaterialUniforms                    ; break; }
            case IUT_ENGINE_UNIFORMS  : { eUniforms = m_Renderer->GetEngineUniformBuffer  (); break; }
            case IUT_RENDERER_UNIFORMS: { eUniforms = m_Renderer->GetRendererUniformBuffer(); break; }
            case IUT_OBJECT_UNIFORMS  : { eUniforms = 0                                     ; break; }
            case IUT_USER_UNIFORMS    : { eUniforms = m_UserUniforms                        ; break; }
            default:
            {
                ERROR("Invalid input type, ignoring.\n");
            }
        }
        
        if(!eUniforms) return "";
        
        string Code;
        Code += LightDataStructure + "\n";
        Code += "\nlayout (std140) uniform " + GetUniformBlockTypeName(Type) + "\n{\n";
        
        i32 uCount = eUniforms->GetUniformCount();
        for(i32 i = 0;i < uCount;i++)
        {
            #define UCase(TypeString,Enum) case Enum: { Code += TypeString; break; }
            switch(eUniforms->GetUniformInfo(i)->Type)
            {
                UCase("\tbool "  ,UniformBuffer::UT_BOOL  );
                UCase("\tint "   ,UniformBuffer::UT_INT   );
                UCase("\tuint "  ,UniformBuffer::UT_UINT  );
                UCase("\tfloat " ,UniformBuffer::UT_FLOAT );
                UCase("\tdouble ",UniformBuffer::UT_DOUBLE);
                UCase("\tvec2 "  ,UniformBuffer::UT_VEC2  );
                UCase("\tvec3 "  ,UniformBuffer::UT_VEC3  );
                UCase("\tvec4 "  ,UniformBuffer::UT_VEC4  );
                UCase("\tmat4 "  ,UniformBuffer::UT_MAT4  );
                UCase("\tLight " ,UniformBuffer::UT_LIGHT );
            }
            Code += eUniforms->GetUniformInfo(i)->Name;
            if(eUniforms->GetUniformInfo(i)->ArraySize != -1) Code += FormatString("[%d];\n",eUniforms->GetUniformInfo(i)->ArraySize);
            else Code += ";\n";
        }
        Code += "};\n";
        
        return Code;
    }
};
