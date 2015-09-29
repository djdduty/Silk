/*
 *  PostProcessingEffect.h
 *  Silk
 *
 *  Created by Michael DeCicco on 9/13/15.
 *
 */

#pragma once
#include <Math/Math.h>
#include <Renderer/ShaderSystem.h>
#include <Renderer/Texture.h>

#include <vector>
using namespace std;

namespace Silk
{
    class Material;
    class Renderer;
    class PostProcessingStage
    {
        public:
            PostProcessingStage(Renderer* r);
            ~PostProcessingStage();
        
            void SetResolution(const Vec2& Res);
            void SetMaterial(Material* Mat);
            void SetInput(ShaderGenerator::OUTPUT_FRAGMENT_TYPE InputType,bool Flag);
            void SetOutputType(Texture::PIXEL_TYPE Type);
            void Execute();
            Texture* GetOutput() const { return m_Output; }
        
        protected:
            friend class PostProcessingEffect;
            void OnResolutionChanged(const Vec2& Resolution);
            
            bool m_UseCustomResolution;
            Vec2 m_Resolution;
            Material* m_Material;
            bool m_Inputs[ShaderGenerator::OFT_COUNT];
        
            Texture::PIXEL_TYPE m_OutputType;
            Texture* m_Output;
        
            Renderer* m_Renderer;
        
    };
    class PostProcessingEffect
    {
        public:
            PostProcessingEffect(Renderer* r) : m_Renderer(r) { }
            ~PostProcessingEffect();
        
            bool LoadEffect(char* Data);
        
            void AddStage(PostProcessingStage* Stage,i32 Iterations = 1);
            void Execute();
        
            Texture* GetOutput() const { return m_Stages[m_Stages.size() - 1]->GetOutput(); }
        
        protected:
            vector<PostProcessingStage*> m_Stages;
            vector<i32> m_StageIterations;
            Renderer* m_Renderer;
    };
};

