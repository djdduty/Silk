#pragma once
#include <Renderer/Material.h>

#include <vector>
using namespace std;

#define PositionAttribIndex                 0
#define NormalAttribIndex                   1
#define TangentAttribIndex                  2
#define ColorAttribIndex                    3
#define TexCoordAttribIndex                 4
#define BoneWeightAttribIndex               5
#define BoneIndexAttribIndex                6
#define InstanceTransformAttribIndex        7
#define InstanceNormalTransformAttribIndex  11
#define InstanceTextureTransformAttribIndex 15

#define PositionAttribName                  "a_Position"
#define NormalAttribName                    "a_Normal"
#define TangentAttribName                   "a_Tangent"
#define ColorAttribName                     "a_Color"
#define TexCoordAttribName                  "a_TexCoord"
#define BoneWeightAttribName                "a_Weights"
#define BoneIndexAttribName                 "a_BoneIDs"
#define InstanceTransformAttribName         "a_InstanceTransform"
#define InstanceNormalTransformAttribName   "a_InstanceNormalTransform"
#define InstanceTextureTransformAttribName  "a_InstanceTextureTransform"

#define PositionOutName   "o_Position"
#define NormalOutName     "o_Normal"
#define TangentOutName    "o_Tangent"
#define ColorOutName      "o_Color"
#define TexCoordOutName   "o_TexCoord"

#define FragmentColorOutputIndex     0
#define FragmentPositionOutputIndex  1
#define FragmentNormalOutputIndex    2
#define FragmentTangentOutputIndex   3
#define FragmentMaterial0OutputIndex 4
#define FragmentMaterial1OutputIndex 5
#define FragmentCustom0OutputIndex   6
#define FragmentCustom1OutputIndex   7
#define FragmentCustom2OutputIndex   8
#define FragmentCustom3OutputIndex   9
#define FragmentCustom4OutputIndex   10
#define FragmentCustom5OutputIndex   11
#define FragmentCustom6OutputIndex   12
#define FragmentCustom7OutputIndex   13
#define FragmentLightOutputIndex     14

#define FragmentColorOutputName     "f_Color"
#define FragmentPositionOutputName  "f_Position"
#define FragmentNormalOutputName    "f_Normal"
#define FragmentTangentOutputName   "f_Tangent"
#define FragmentMaterial0OutputName "f_Material0"
#define FragmentMaterial1OutputName "f_Material1"
#define FragmentCustom0OutputName   "f_Custom0"
#define FragmentCustom1OutputName   "f_Custom1"
#define FragmentCustom2OutputName   "f_Custom2"
#define FragmentCustom3OutputName   "f_Custom3"
#define FragmentCustom4OutputName   "f_Custom4"
#define FragmentCustom5OutputName   "f_Custom5"
#define FragmentCustom6OutputName   "f_Custom6"
#define FragmentCustom7OutputName   "f_Custom7"
#define FragmentLightOutputName     "f_LightAccum"

namespace Silk
{
    class Shader;
    class Renderer;
    class UniformBuffer;
    class ModelUniformSet;
    class RenderUniformSet;
    class MaterialUniformSet;
    
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
                IAT_INSTANCE_TRANSFORM,
                IAT_INSTANCE_NORMAL_TRANSFORM,
                IAT_INSTANCE_TEXTURE_TRANSFORM,
                IAT_COUNT,
            };
            enum OUTPUT_FRAGMENT_TYPE
            {
                OFT_COLOR,
                OFT_POSITION,
                OFT_NORMAL,
                OFT_TANGENT,
                OFT_MATERIAL0,
                OFT_MATERIAL1,
                OFT_CUSTOM0,
                OFT_CUSTOM1,
                OFT_CUSTOM2,
                OFT_CUSTOM3,
                OFT_CUSTOM4,
                OFT_CUSTOM5,
                OFT_CUSTOM6,
                OFT_CUSTOM7,
                OFT_LIGHTACCUM,
                OFT_COUNT,
            };
            enum LIGHTING_MODES
            {
                //Basic phong lighting
                LM_PHONG,
                
                //Don't calculate lighting
                LM_FLAT,
                
                //Pass surface attributes to output color buffers
                LM_PASS,
            };
            enum PARALLAX_FUNCTION
            {
                PF_RELIEF,
                PF_OCCLUSION,
            };
        
            struct CodeBlock
            {
                string ID;
                string Code;
                i32 Index;
            };
        
            ShaderGenerator(Renderer* r);
            ~ShaderGenerator();
        
            void Reset();
        
            void AddVertexModule  (CString Code,i32 Index);
            void AddGeometryModule(CString Code,i32 Index);
            void AddFragmentModule(CString Code,i32 Index);
        
            void AddVertexFunction  (CString Code,i32 Index);
            void AddGeometryFunction(CString Code,i32 Index);
            void AddFragmentFunction(CString Code,i32 Index);
        
            void SetAllowInstancing(bool Flag);
            void SetAllowInstancedTextureMatrix(bool Flag) { m_AllowInstancedTextureMatrix = Flag; }
            void SetTextureInput   (Material::MAP_TYPE   Type,bool Flag) { m_MapTypesUsed        [Type] = Flag; }
            void SetUniformInput   (INPUT_UNIFORM_TYPE   Type,bool Flag) { m_UniformInputsUsed   [Type] = Flag; }
            void SetAttributeInput (INPUT_ATTRIBUTE_TYPE Type,bool Flag) { m_AttributeInputsUsed [Type] = Flag; }
            void SetAttributeOutput(INPUT_ATTRIBUTE_TYPE Type,bool Flag) { m_AttributeOutputsUsed[Type] = Flag; }
            void SetFragmentOutput (OUTPUT_FRAGMENT_TYPE Type,bool Flag) { m_FragmentOutputsUsed [Type] = Flag; }
        
            void SetShaderVersion(i32 Version) { m_ShaderVersion = Version; }
        
            void SetUserUniformBuffer    (UniformBuffer* UserUniforms    ) { m_UserUniforms     = UserUniforms    ; }
        
            void SetLightingMode(LIGHTING_MODES Type) { m_LightingMode = Type; }
            void SetParallaxFunction(PARALLAX_FUNCTION Func) { m_ParallaxFunction = Func; }
			void SetParallaxSoftShadowing(bool Flag) { m_UseParallaxShadows = Flag; }
            
            Shader* Generate();
        
        protected:
            string GenerateVertexShader  ();
            string GenerateGeometryShader();
            string GenerateFragmentShader();
        
            i32 ReadBlocks(CString Code,i32 Index,i32 ShaderType);
            i32 ParseBlock(CString Code,i32 StartIndex,i32 ExecutionIndex,i32 ShaderType);
            string GenerateInputBlock(INPUT_UNIFORM_TYPE Type);
        
            vector<CodeBlock>   m_VertexBlocks;
            vector<CodeBlock>   m_GeometryBlocks;
            vector<CodeBlock>   m_FragmentBlocks;
        
            vector<CodeBlock>   m_VertexFuncs;
            vector<CodeBlock>   m_GeometryFuncs;
            vector<CodeBlock>   m_FragmentFuncs;
            
            i32                 m_ShaderVersion;

            bool                m_AllowInstancing;
            bool                m_AllowInstancedTextureMatrix;
        
            bool                m_MapTypesUsed        [Material::MT_COUNT];
            bool                m_UniformInputsUsed   [IUT_COUNT         ];
            bool                m_AttributeInputsUsed [IAT_COUNT         ];
            bool                m_AttributeOutputsUsed[IAT_COUNT         ];
            bool                m_FragmentOutputsUsed [OFT_COUNT         ];
        
            UniformBuffer*      m_UserUniforms        ;
            ModelUniformSet*    m_NullModelUniforms   ;
            MaterialUniformSet* m_NullMaterialUniforms;
            LIGHTING_MODES      m_LightingMode        ;
            PARALLAX_FUNCTION   m_ParallaxFunction    ;
			bool				m_UseParallaxShadows  ;
        
            Renderer* m_Renderer;
            i32 m_ShadersGenerated;
    };
    string GetUniformBlockTypeName  (ShaderGenerator::INPUT_UNIFORM_TYPE   Type);
    string GetAttributeTypeName     (ShaderGenerator::INPUT_ATTRIBUTE_TYPE Type);
    string GetFragmentOutputTypeName(ShaderGenerator::OUTPUT_FRAGMENT_TYPE Type);
    i32    GetFragmentOutputIndex   (ShaderGenerator::OUTPUT_FRAGMENT_TYPE Type);
};

