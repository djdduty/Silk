/*
 *  PostProcessingEffect.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 9/13/15.
 *
 */

#include <Renderer/PostProcessingEffect.h>
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>

#include <Utilities/Parse.h>

namespace Silk
{
    PostProcessingStage::PostProcessingStage(Renderer* r) : m_OutputType(Texture::PT_UNSIGNED_BYTE), m_Output(0), m_Renderer(r)
    {
        m_Output = m_Renderer->GetRasterizer()->CreateTexture();
        m_Material = 0;
        
        for(i32 i = 0;i < ShaderGenerator::OFT_COUNT;i++) m_Inputs[i] = false;
    }
    PostProcessingStage::~PostProcessingStage()
    {
        m_Output->Destroy();
    }
    void PostProcessingStage::SetResolution(const Vec2 &Res)
    {
         m_UseCustomResolution = true;
         m_Resolution = Res;
         m_Output->CreateTexture(m_Resolution.x,m_Resolution.y,m_OutputType);
         m_Output->UpdateTexture();
    }
    void PostProcessingStage::SetMaterial(Material *Mat)
    {
        if(m_Material) m_Material->Destroy();
        m_Material = Mat;
        m_Material->AddRef();
    }
    void PostProcessingStage::SetInput(ShaderGenerator::OUTPUT_FRAGMENT_TYPE InputType,bool Flag)
    {
        m_Inputs[InputType] = Flag;
    }
    void PostProcessingStage::SetOutputType(Texture::PIXEL_TYPE Type)
    {
        if(Type != m_Output->GetPixelType())
        {
            m_Output->CreateTexture(m_Resolution.x,m_Resolution.y,Type);
            m_Output->UpdateTexture();
        }
        m_OutputType = Type;
    }
    void PostProcessingStage::Execute()
    {
        if(!m_Material) return;
        
        //Use scene output as main input (materials don't have to use all scene outputs)
        m_Renderer->GetSceneOutput()->EnableTexture(m_Material);
        
        //Render using the effect
        m_Output->EnableRTT(false);
        m_Renderer->RenderTexture(0,m_Material);
        m_Output->DisableRTT();
        
        m_Renderer->RenderTexture(m_Output);
    }
    void PostProcessingStage::OnResolutionChanged(const Vec2 &Resolution)
    {
        if(m_UseCustomResolution) return;
        m_Resolution = Resolution;
        m_Output->CreateTexture(Resolution.x,Resolution.y,m_OutputType);
        m_Output->UpdateTexture();
    }
    
    bool PostProcessingEffect::LoadEffect(char* Data)
    {
        i32 dPos = 0;
        while(Data[dPos] != 0)
        {
            string Word = ReadNextWord(Data,"",dPos);
            if(Word == "Effect")
            {
                string EffectName = ReadNextString(Data,dPos);
                
                if(!ReadUntil(Data,"{",dPos)) { ERROR("Unexpected end-of-file while searching for '{' for \"Effect\".\n"); return false; }
                string EffStr;
                i32 eIdx = 0;
                if(!ReadUntil(Data,"}",dPos,EffStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"Effect\".\n"); return false; }
                
                while(eIdx < EffStr.length())
                {
                    Word = ReadNextWord(&EffStr[0],"",eIdx);
                    if(Word == "Stage")
                    {
                        if(!ReadUntil(&EffStr[0],"{",eIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Stage\".\n"); return false; }
                        string StageStr;
                        i32 sIdx = 0;
                        if(!ReadUntil(&EffStr[0],"}",eIdx,StageStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"Stage\".\n"); return false; }
                        
                        PostProcessingStage* Stage = new PostProcessingStage(m_Renderer);
                        Stage->m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
                        
                        i32 Iterations = 1;
                        while(sIdx < StageStr.length())
                        {
                            Word = ReadNextWord(&StageStr[0],"",sIdx);
                            if(Word == "Material")
                            {
                                Material* Mat = m_Renderer->CreateMaterial();
                                if(!Mat->LoadMaterial(&(&StageStr[0])[sIdx - 9],&sIdx))
                                {
                                    delete Stage;
                                    Mat->Destroy();
                                    return false;
                                }
                                Stage->SetMaterial(Mat);
                                Mat->Destroy();
                            }
                            else if(Word == "Iterations") Iterations = ReadNextInt32(&StageStr[0],sIdx);
                            else if(Word == "Requires")
                            {
                                if(!ReadUntil(&StageStr[0],"{",sIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Requires\".\n"); return false; }
                                string ReqStr;
                                i32 rIdx = 0;
                                if(!ReadUntil(&StageStr[0],"}",sIdx,ReqStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"Requires\".\n"); return false; }
                                while(rIdx < ReqStr.length())
                                {
                                    Word = ReadNextWord(&ReqStr[0],"_1234567890",rIdx);
                                    
                                    for(i32 on = 0;on < ShaderGenerator::OFT_COUNT;on++)
                                    {
                                        if(Word == GetFragmentOutputTypeName((ShaderGenerator::OUTPUT_FRAGMENT_TYPE)on))
                                        {
                                            Stage->SetInput((ShaderGenerator::OUTPUT_FRAGMENT_TYPE)on,true);
                                        }
                                    }
                                }
                            }
                            else if(Word == "OutputType")
                            {
                                Word = ReadNextWord(&StageStr[0],"",sIdx);
                                if(Word == "float") Stage->SetOutputType(Texture::PT_FLOAT);
                                if(Word == "ubyte") Stage->SetOutputType(Texture::PT_UNSIGNED_BYTE);
                            }
                        }
                        AddStage(Stage,Iterations);
                    }
                }
                return true;
            }
        }
        return false;
    }
        
    void PostProcessingEffect::Execute()
    {
        for(i32 i = 0;i < m_Stages.size();i++)
        {
            for(i32 it = 0;it < m_StageIterations[i];it++)
            {
                m_Stages[i]->Execute();
            }
        }
    }
};