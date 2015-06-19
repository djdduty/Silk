#pragma once
#include <Raster/Raster.h>
#include <Renderer/Renderer.h>

#include <vector>
using namespace std;

namespace Silk
{
    class ShaderGenerator
    {
        public:
            enum INPUT_UNIFORM_TYPE
            {
                IUT_MATERIAL_UNIFORMS,
                IUT_ENGINE_UNIFORMS,
                IUT_RENDERER_UNIFORMS,
                IUT_OBJECT_UNIFORMS,
                IUT_USER_UNIFORMS,
                IUT_COUNT,
            };
            enum INPUT_ATTRIBUTE_TYPE
            {
                IAT_POSITION,
                IAT_NORMAL,
                IAT_TANGENT,
                IAT_COLOR,
                IAT_TEXCOORD,
                IAT_BONE_IDX,
                IAT_BONE_WEIGHT,
                IAT_COUNT,
            };
        
            struct CodeBlock
            {
                string ID;
                string Code;
                i32 Index;
            };
        
            ShaderGenerator(Renderer* r);
            ~ShaderGenerator();
        
            void AddModule(CString Code,i32 Index);
            void SetUniformInput(INPUT_UNIFORM_TYPE Type,bool Flag) { m_UniformInputsUsed[Type] = Flag; }
            void SetAttributeInput(INPUT_ATTRIBUTE_TYPE Type,bool Flag) { m_AttributeInputsUsed[Type] = Flag; }
            void SetShaderVersion(i32 Version) { m_ShaderVersion = Version; }
            void SetMaterialUniformBuffer(UniformBuffer* MaterialUniforms) { m_MaterialUniforms = MaterialUniforms; }
            void SetUserUniformBuffer(UniformBuffer* UserUniforms) { m_UserUniforms = UserUniforms; }
            
            Shader* Generate();
        
        protected:
            i32 ReadBlocks(CString Code,i32 Index);
            i32 ParseBlock(CString Code,i32 StartIndex,i32 ExecutionIndex);
            string GenerateInputBlock(INPUT_UNIFORM_TYPE Type);
        
            vector<CodeBlock> m_Blocks;
            i32 m_ShaderVersion;
            bool m_UniformInputsUsed[IUT_COUNT];
            bool m_AttributeInputsUsed[IAT_COUNT];
            UniformBuffer* m_MaterialUniforms;
            UniformBuffer* m_UserUniforms;
        
            Renderer* m_Renderer;
    };
};

