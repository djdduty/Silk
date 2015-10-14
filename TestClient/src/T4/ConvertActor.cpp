#pragma once
#include <T4/ConvertActor.h>
#include <T4/SOIL/SOIL.h>
using namespace Silk;
static vector<Texture*> Textures    ;
static vector<string>   TextureFiles;

Texture* LoadTexture(Renderer* r,const string& Filename)
{
    for(i32 i = 0;i < TextureFiles.size();i++)
    {
        if(TextureFiles[i].length() != Filename.length()) continue;
        if(TextureFiles[i] != Filename) continue;
        
        return Textures[i];
    }
    
    i32 w,h,ch;
    Byte* Data = (Byte*)SOIL_load_image(Filename.c_str(),&w,&h,&ch,4);
    
    if(Data)
    {
        Texture* t = r->GetRasterizer()->CreateTexture();
        t->CreateTextureb(w,h,Data);
        for(int x = 0;x < w;x++)
        {
            for(int y = 0;y < h;y++)
            {
                Vec4 c = t->GetPixel(Vec2(x,y));
                if(c.w < 1.0f && c.w != 0.0f) c.w = 1.0f;
                t->SetPixel(Vec2(x,y),c);
            }
        }
        t->UpdateTexture();
        delete [] Data;
        TextureFiles.push_back(Filename);
        Textures.push_back(t);
        return t;
    }
    
    return 0;
}

static vector<vector<Mesh*   >> ActorMeshes  ;
static vector<vector<Texture*>> ActorTextures;
static vector<string>           ActorFiles   ;

void LoadActorToEngine(Renderer* r,Shader* Shdr,const Mat4& Transform,i32 ID,Actor* a,vector<RenderObject*>& OutObjs)
{
    string fn = a->GetFilename();
    
    ActorMesh* m = a->GetMesh();
    if(!m) return;
    
    i32 ActorIdx = -1;
    if(a->GetDef())
    {
        for(i32 i = 0;i < ActorFiles.size();i++)
        {
            if(ActorFiles[i].length() != fn.length()) continue;
            if(ActorFiles[i] != fn) continue;
            ActorIdx = i;
            break;
        }
    }
    
    if(ActorIdx != -1)
    {
        for(i32 i = 0;i < ActorMeshes[ActorIdx].size();i++)
        {
            Mesh   * m = ActorMeshes  [ActorIdx][i];
            Texture* t = ActorTextures[ActorIdx][i];
            
            RenderObject* Obj = r->CreateRenderObject(ROT_MESH);
            Obj->SetTransform(Transform);
        
            Material* Mat = r->CreateMaterial();
            Mat->SetShader(Shdr);
            Mat->SetMap(Material::MT_DIFFUSE,t);
            
            Obj->SetMesh(m,Mat);
            Obj->SetObjectID(ID);
            r->GetScene()->AddRenderObject(Obj);
            
            OutObjs.push_back(Obj);
        }
    }
    else
    {
        if(a->GetDef())
        {
            ActorFiles.push_back(fn);
            ActorMeshes.push_back(vector<Mesh*>());
            ActorTextures.push_back(vector<Texture*>());
        }
        
        for(i32 s = 0;s < m->GetSubMeshCount();s++)
        {
            Texture* t = 0;
            if(s < m->m_MeshInfos.size())
            {
                if(m->m_MeshInfos[s].TSNR_ID != -1)
                {
                    int TexID = m->m_TXSTs[m->m_TSNRs[m->m_MeshInfos[s].TSNR_ID].TXST_ID].TextureID;
                    if(TexID < m->m_Textures.size()) t = LoadTexture(r,m->m_Textures[TexID]);
                }
            }
            
            Turok4::SubMesh* sm = m->GetSubMesh(s);
            if(sm->GetVertexCount() != 0)
            {
                vector<Vec2> TexCs;
                vector<Vec3> Verts,Norms;
                vector<Vec4> Colors;
                vector<i32 > Indices;
            
                for(i32 vIdx = 0;vIdx < sm->GetVertexCount();vIdx++)
                {
                    Vec2 t0;
                    Vec3 v0,n0;
                    sm->GetTexCoord(vIdx,&t0.x);
                    sm->GetVertex(vIdx,&v0.x);
                    sm->GetNormal(vIdx,&n0.x);
                    
                    TexCs .push_back(t0);
                    Verts .push_back(v0);
                    Norms .push_back(n0);
                    Colors.push_back(Vec4(1,1,1,1));
                }
                
                if(sm->GetIndexCount() != 0)
                {
                    for(i32 idx = 0;idx < sm->GetIndexCount();idx++)
                    {
                        i32 I = sm->GetIndex(idx);
                        Indices.push_back(I);
                    }
                    
                    Mesh* M = new Mesh();
                    if(Indices.size() != 0) M->SetIndexBuffer   (Indices.size(),&Indices[0]  );
                    M->SetVertexBuffer  (Verts  .size(),&Verts  [0].x);
                    M->SetNormalBuffer  (Norms  .size(),&Norms  [0].x);
                    M->SetTexCoordBuffer(TexCs  .size(),&TexCs  [0].x);
                    M->PrimitiveType = PT_TRIANGLE_STRIP;
                
                    Material* Mat = r->CreateMaterial();
                    Mat->SetShader(Shdr);
                    Mat->SetMap(Material::MT_DIFFUSE,t);
                    
                    RenderObject* Obj = r->CreateRenderObject(ROT_MESH);
                    Obj->SetMesh(M,Mat);
                    Obj->SetObjectID(ID);
                    Obj->SetTransform(Transform);
                    
                    OutObjs.push_back(Obj);
                    r->GetScene()->AddRenderObject(Obj);
                    
                    if(a->GetDef())
                    {
                        ActorMeshes  [ActorMeshes  .size() - 1].push_back(M);
                        ActorTextures[ActorTextures.size() - 1].push_back(t);
                    }
                }
                else if(sm->GetChunkCount() != 0)
                {
                    for(i32 ch = 0;ch < sm->GetChunkCount();ch++)
                    {
                        Turok4::MeshChunk* Chunk = sm->GetChunk(ch);
                        
                        if(ch < m->m_MTRLs.size())
                        {
                            if(m->m_MTRLs[ch].Unk4 >= 0 && m->m_MTRLs[ch].Unk4 < m->m_TSNRs.size())
                            {
                                int TexID = m->m_TXSTs[m->m_TSNRs[m->m_MTRLs[ch].Unk4].TXST_ID].TextureID;
                                if(TexID < m->m_Textures.size()) t = LoadTexture(r,m->m_Textures[TexID]);
                            }
                        }
                        
                        Indices.clear();
                        for(i32 idx = 0;idx < Chunk->GetIndexCount();idx++)
                        {
                            i32 I = Chunk->GetIndex(idx);
                            Indices.push_back(I);
                        }
                    
                        Mesh* M = new Mesh();
                        if(Indices.size() != 0) M->SetIndexBuffer(Indices.size(),&Indices[0]  );
                        M->SetVertexBuffer  (Verts.size(),&Verts[0].x);
                        M->SetNormalBuffer  (Norms.size(),&Norms[0].x);
                        M->SetTexCoordBuffer(TexCs.size(),&TexCs[0].x);
                        M->PrimitiveType = PT_TRIANGLE_STRIP;
                    
                        Material* Mat = r->CreateMaterial();
                        Mat->SetShader(Shdr);
                        Mat->SetMap(Material::MT_DIFFUSE,t);
                        
                        RenderObject* Obj = r->CreateRenderObject(ROT_MESH);
                        Obj->SetMesh(M,Mat);
                        Obj->SetTransform(Transform);
                        Obj->SetObjectID(ID);
                        
                        r->GetScene()->AddRenderObject(Obj);
                        OutObjs.push_back(Obj);
                        
                        if(a->GetDef())
                        {
                            ActorMeshes  [ActorMeshes  .size() - 1].push_back(M);
                            ActorTextures[ActorTextures.size() - 1].push_back(t);
                        }
                    }
                }
            }
        }
    }
}
void DebugPrintActorInfo()
{
    printf("Textures:\n");
    for(i32 i = 0;i < TextureFiles.size();i++) printf("%s\n",TextureFiles[i].c_str());
    printf("Actors:\n");
    for(i32 i = 0;i < ActorFiles.size();i++) printf("%s\n",ActorFiles[i].c_str());
}