#include <Renderer/Material.h>
#include <Renderer/ShaderSystem.h>
#include <Renderer/Renderer.h>
#include <Raster/Raster.h>
#include <Utilities/Parse.h>

namespace Silk
{
    bool Material::LoadMaterial(char *Data,i32* Offset)
    {
        i32 dPos = 0;
        while(Data[dPos] != 0)
        {
            string Word = ReadNextWord(Data,"",dPos);
            if(Word == "Material")
            {
                if(!ReadUntil(Data,"{",dPos)) { ERROR("Unexpected end-of-file while searching for '{' for \"Material\".\n"); return false; }
                string Mtrl;
                i32 mIdx = 0;
                if(!ReadUntil(Data,"}",dPos,Mtrl)) { ERROR("Unexpected end-of-file while searching for '}' for \"Material\".\n"); return false; }
               
                 while(mIdx < Mtrl.length())
                 {
                    Word = ReadNextWord(&Mtrl[0],"",mIdx);
                    if(Word == "Shader")
                    {
                        if(!ReadUntil(&Mtrl[0],"{",mIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Shader\".\n"); return false; }
                        
                        string Sh;
                        i32 ShIdx = 0;
                        
                        if(!ReadUntil(&Mtrl[0],"}",mIdx,Sh)) { ERROR("Unexpected end-of-file while searching for '}' for \"Shader\".\n"); return false; }
                    
                        ShaderGenerator* gen = m_Renderer->GetShaderGenerator();
                        gen->Reset();
                        
                        vector<string> UserUniformBlocks;
                        vector<vector<string>> UserUniforms;
                        
                        while(ShIdx < Sh.length())
                        {
                            Word = ReadNextWord(&Sh[0],"",ShIdx);
                            if(Word.length() == 0) break;
                            if(Word == "VertexModule")
                            {
                                string BlockID = ReadNextString(&Sh[0],ShIdx);
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"VertexModule\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"VertexModule\".\n"); return false; }
                                
                                gen->AddVertexModule(const_cast<CString>((string("[") + BlockID + "]" + Code + "[/" + BlockID + "]").c_str()),Idx);
                            }
                            else if(Word == "VertexFunction")
                            {
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"VertexFunction\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"VertexFunction\".\n"); return false; }
                                
                                gen->AddVertexFunction(const_cast<CString>(Code.c_str()),Idx);
                            }
                            else if(Word == "GeometryModule")
                            {
                                string BlockID = ReadNextString(&Sh[0],ShIdx);
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"GeometryModule\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"GeometryModule\".\n"); return false; }
                                
                                gen->AddGeometryModule(const_cast<CString>((string("[") + BlockID + "]" + Code + "[/" + BlockID + "]").c_str()),Idx);
                            }
                            else if(Word == "GeometryFunction")
                            {
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"GeometryFunction\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"GeometryFunction\".\n"); return false; }
                                
                                gen->AddGeometryFunction(const_cast<CString>(Code.c_str()),Idx);
                            }
                            else if(Word == "FragmentModule")
                            {
                                string BlockID = ReadNextString(&Sh[0],ShIdx);
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"FragmentModule\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"FragmentModule\".\n"); return false; }
                                
                                gen->AddFragmentModule(const_cast<CString>((string("[") + BlockID + "]" + Code + "[/" + BlockID + "]").c_str()),Idx);
                            }
                            else if(Word == "FragmentFunction")
                            {
                                i32 Idx = ReadNextInt32(&Sh[0],ShIdx);
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx     )) { ERROR("Unexpected end-of-file while searching for '{' for \"FragmentFunction\".\n"); return false; }
                                string Code;
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Code)) { ERROR("Unexpected end-of-file while searching for '}' for \"FragmentFunction\".\n"); return false; }
                                
                                gen->AddFragmentFunction(const_cast<CString>(Code.c_str()),Idx);
                            }
                            else if(Word == "Uniforms")
                            {
                                string Name = ReadNextString(&Sh[0],ShIdx);
                                string Vars;
                                
                                if(!ReadUntil(&Sh[0],"{",ShIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Uniforms\".\n"); return false; }
                                if(!ReadUntil(&Sh[0],"}",ShIdx,Vars)) { ERROR("Unexpected end-of-file while searching for '}' for \"Uniforms\".\n"); return false; }
                                UserUniformBlocks.push_back(Name);
                                UserUniforms.push_back(vector<string>());
                                
                                i32 vIdx = 0;
                                while(vIdx < Vars.length())
                                {
                                    string v;
                                    ReadUntil(&Vars[0],"\n",vIdx,v);
                                    if(v.length() > 0) UserUniforms[UserUniforms.size() - 1].push_back(v);
                                }
                            }
                            else if(Word == "TextureMaps")
                            {
                                if(!ReadUntil(&Sh[0],"{",ShIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"TextureMaps\".\n"); return false; }
                                
                                string ReqStr;
                                i32 rIdx = 0;
                                
                                if(!ReadUntil(&Sh[0],"}",ShIdx,ReqStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"TextureMaps\".\n"); return false; }
                            
                                while(rIdx < ReqStr.length())
                                {
                                    Word = ReadNextWord(&ReqStr[0],"_1234567890",rIdx);
                                    
                                    for(i32 mn = 0;mn < Material::MT_COUNT;mn++)
                                    {
                                        if(Word == GetShaderMapName((Material::MAP_TYPE)mn))
                                        {
                                            gen->SetTextureInput((Material::MAP_TYPE)mn,true);
                                            if(mn == MT_FRAG_LIGHTACCUM)
                                            {
                                                SetMap(MT_FRAG_LIGHTACCUM,m_Renderer->GetLightAccumulationTexture());
                                            }
                                        }
                                    }
                                }
                            }
                            else if(Word == "Requires")
                            {
                                if(!ReadUntil(&Sh[0],"{",ShIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Requires\".\n"); return false; }
                                
                                string ReqStr;
                                i32 rIdx = 0;
                                
                                if(!ReadUntil(&Sh[0],"}",ShIdx,ReqStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"Requires\".\n"); return false; }
                            
                                while(rIdx < ReqStr.length())
                                {
                                    Word = ReadNextWord(&ReqStr[0],"_1234567890",rIdx);
                                    if(Word.length() == 0) break;
                                    for(i32 un = 0;un < ShaderGenerator::IUT_COUNT;un++)
                                    {
                                        if(Word == GetUniformBlockTypeName((ShaderGenerator::INPUT_UNIFORM_TYPE)un))
                                        {
                                            gen->SetUniformInput((ShaderGenerator::INPUT_UNIFORM_TYPE)un,true);
                                            break;
                                        }
                                    }
                                    
                                    for(i32 an = 0;an < ShaderGenerator::IAT_COUNT;an++)
                                    {
                                        if(Word == GetAttributeTypeName((ShaderGenerator::INPUT_ATTRIBUTE_TYPE)an))
                                        {
                                            gen->SetAttributeInput ((ShaderGenerator::INPUT_ATTRIBUTE_TYPE)an,true);
                                            break;
                                        }
                                    }
                                    
                                    for(i32 un = 0;un < UserUniforms.size();un++)
                                    {
                                        if(Word == UserUniformBlocks[un])
                                        {
                                            gen->SetUniformInput(ShaderGenerator::IUT_USER_UNIFORMS,true);
                                            m_UserUniforms = m_Renderer->GetRasterizer()->CreateUniformBuffer(ShaderGenerator::IUT_USER_UNIFORMS);
                                            
                                            for(i32 u = 0;u < UserUniforms[un].size();u++)
                                            {
                                                string v = UserUniforms[un][u];
                                                i32 vIdx = 0;
                                                string Type = ReadNextWord(&v[0],"1234567890",vIdx);
                                                string Name = ReadNextWord(&v[0],"_1234567890",vIdx);
                                                bool HasInitializer = ReadUntil(&v[0],"=",vIdx);
                                                
                                                if(Type == "int")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    i32 Value = 0;
                                                    if(HasInitializer) Value = ReadNextInt32(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "uint")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    u32 Value = 0;
                                                    if(HasInitializer) Value = ReadNextInt32(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "float")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    f32 Value = 0;
                                                    if(HasInitializer) Value = ReadNextFloat32(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "double")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    f64 Value = 0;
                                                    if(HasInitializer) Value = ReadNextFloat64(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "vec2")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    Vec2 Value;
                                                    if(HasInitializer) Value = ReadNextVec2(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "vec3")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    Vec3 Value;
                                                    if(HasInitializer) Value = ReadNextVec3(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "vec4")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    Vec4 Value;
                                                    if(HasInitializer) Value = ReadNextVec4(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                                else if(Type == "mat4")
                                                {
                                                    i32 ID = m_UserUniforms->DefineUniform(Name);
                                                    Mat4 Value = Mat4::Identity;
                                                    if(HasInitializer) Value = ReadNextMat4(&v[0],vIdx);
                                                    m_UserUniforms->SetUniform(ID,Value);
                                                }
                                            }
                                            
                                            gen->SetUserUniformBuffer(m_UserUniforms);
                                            break;
                                        }
                                    }
                                }
                            }
                            else if(Word == "Outputs")
                            {
                                if(!ReadUntil(&Sh[0],"{",ShIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"Outputs\".\n"); return false; }
                                
                                string OutStr;
                                i32 oIdx = 0;
                                
                                if(!ReadUntil(&Sh[0],"}",ShIdx,OutStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"Outputs\".\n"); return false; }
                            
                                while(oIdx < OutStr.length())
                                {
                                    Word = ReadNextWord(&OutStr[0],"_",oIdx);
                                    if(Word.length() == 0) break;
                                    for(i32 on = 0;on < ShaderGenerator::OFT_COUNT;on++)
                                    {
                                        if(Word == GetFragmentOutputTypeName((ShaderGenerator::OUTPUT_FRAGMENT_TYPE)on))
                                        {
                                            gen->SetFragmentOutput((ShaderGenerator::OUTPUT_FRAGMENT_TYPE)on,true);
                                        }
                                    }
                                }
                            }
                            else if(Word == "VertexOutputs")
                            {
                                if(!ReadUntil(&Sh[0],"{",ShIdx)) { ERROR("Unexpected end-of-file while searching for '{' for \"VertexOutputs\".\n"); return false; }
                                
                                string OutStr;
                                i32 oIdx = 0;
                                
                                if(!ReadUntil(&Sh[0],"}",ShIdx,OutStr)) { ERROR("Unexpected end-of-file while searching for '}' for \"VertexOutputs\".\n"); return false; }
                            
                                while(oIdx < OutStr.length())
                                {
                                    Word = ReadNextWord(&OutStr[0],"_",oIdx);
                                    if(Word.length() == 0) break;
                                    for(i32 an = 0;an < ShaderGenerator::IAT_COUNT;an++)
                                    {
                                        if(Word == GetAttributeTypeName((ShaderGenerator::INPUT_ATTRIBUTE_TYPE)an))
                                        {
                                            gen->SetAttributeOutput ((ShaderGenerator::INPUT_ATTRIBUTE_TYPE)an,true);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        
                        Shader* Shdr = gen->Generate();
                        SetShader(Shdr);
                        Shdr->Destroy();
                    }
                }
                if(Offset) *Offset += dPos;
                return true;
            }
        }
        return false;
    }
};
