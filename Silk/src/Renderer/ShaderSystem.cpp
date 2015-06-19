#include <Renderer/ShaderSystem.h>
#include <Utilities/Utilities.h>

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
    
    ShaderGenerator::ShaderGenerator(Renderer* r) : m_ShaderVersion(330), m_MaterialUniforms(0), m_UserUniforms(0), m_Renderer(r)
    {
        for(i32 i = 0;i < IAT_COUNT;i++) m_AttributeInputsUsed[i] = false;
        for(i32 i = 0;i < IUT_COUNT;i++) m_UniformInputsUsed  [i] = false;
        m_AttributeInputsUsed[0] = true;
    }
    ShaderGenerator::~ShaderGenerator()
    {
    }
    
    

    void ShaderGenerator::AddModule(CString Code,i32 Index)
    {
        if(ReadBlocks(Code,Index) < 0) ERROR("Invalid shader module, no code blocks found.\n");
    }
    Shader* ShaderGenerator::Generate()
    {
        CodeBlock* SetPosition  = 0;
        CodeBlock* SetNormal    = 0;
        CodeBlock* SetTangent   = 0;
        CodeBlock* SetColor     = 0;
        CodeBlock* SetTexC      = 0;
        CodeBlock* SetRoughness = 0;
        CodeBlock* SetMetalness = 0;
        
        vector<i32> UnsortedBlockIndices;
        
        for(i32 i = 0;i < m_Blocks.size();i++)
        {
            if     (m_Blocks[i].ID == "SetPosition"      ) SetPosition  = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetNormal"        ) SetNormal    = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetTangent"       ) SetTangent   = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetColor"         ) SetColor     = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetTexCoords"     ) SetTexC      = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetRoughness"     ) SetRoughness = &m_Blocks[i];
            else if(m_Blocks[i].ID == "SetMetalness"     ) SetMetalness = &m_Blocks[i];
            
            UnsortedBlockIndices.push_back(i);
        }
        
        /* Default code dependencies */
        if(!SetPosition            ) m_UniformInputsUsed[IUT_RENDERER_UNIFORMS] = true;
        if(!SetNormal || SetTangent) m_UniformInputsUsed[IUT_OBJECT_UNIFORMS  ] = true;
        
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
        
        if(!SetPosition) VertexShader += string("\tgl_Position = u_MVP * vec4(") + PositionAttribName + ",1.0);\n\t" +
                                                        PositionOutName + " = "                       + PositionAttribName  + ";\n";
        if(!SetNormal  ) VertexShader += string("\t") + NormalOutName   + " = u_NormalMatrix * vec4(" + NormalAttribName    + ",1.0f);\n";
        if(!SetTangent ) VertexShader += string("\t") + TangentOutName  + " = u_NormalMatrix * vec4(" + TangentAttribName   + ",1.0f);\n";
        if(!SetColor   ) VertexShader += string("\t") + ColorOutName    + " = " + ColorAttribName + ";\n";
        if(!SetTexC    ) VertexShader += string("\t") + TexCoordOutName + " = u_TextureMatrix * vec3(" + TexCoordAttribName + ",1.0f);\n";
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
                if(m_Blocks[UnsortedBlockIndices[i]].Index < m_Blocks[MinExecutionIndexIndex].Index)
                {
                    MinExecutionIndexIndex = UnsortedBlockIndices[i];
                    BlockIndexIndex = i;
                }
            }
            UnsortedBlockIndices.erase(UnsortedBlockIndices.begin() + BlockIndexIndex);
            VertexShader += "\t" + m_Blocks[MinExecutionIndexIndex].Code + "\n";
        }
        
        VertexShader += "}\n";
        
        
        printf("Shader generated:\n%s",VertexShader.c_str());
        
        return 0;
    }
    i32 ShaderGenerator::ReadBlocks(CString Code,i32 Index)
    {
        i32 p = 0;
        while(Code[p] != 0)
        {
            if(Code[p] == '[')
            {
                i32 r = ParseBlock(Code,p + 1,Index);
                if(r == -1) return -1;
                
                p = r;
            }
            p++;
        }
        return 0;
    }
    i32 ShaderGenerator::ParseBlock(CString Code,i32 StartIndex,i32 ExecutionIndex)
    {
        i32 p = StartIndex;
        
        CodeBlock b;
        while(Code[p] != 0 && Code[p] != ']') { b.ID += Code[p]; p++; }
        if(Code[p] ==  0 ) { ERROR("Unexpected end of shader template file.\n"); return -1; }
        p++;
        string BlockCode;
        while(Code[p] != 0 && (Code[p] != '[' && Code[p + 1] != '/')) { b.Code += Code[p]; p++; }
        if(Code[p] ==  0 ) { ERROR("Unexpected end of shader template file.\n"); return -1; }
        
        b.Index = ExecutionIndex;
        
        m_Blocks.push_back(b);
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
            case IUT_USER_UNIFORMS    : { eUniforms = m_UserUniforms                        ; break; }
            default:
            {
                ERROR("Invalid input type, ignoring.\n");
            }
        }
        
        if(!eUniforms) return "";
        
        string Code;
        Code += "\nlayout (packed) uniform " + BlockNames[Type] + "\n{\n";
        
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
            }
            Code += eUniforms->GetUniformInfo(i)->Name + ";\n";
        }
        Code += "};\n";
        return Code;
    }
};
