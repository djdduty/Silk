/*
 *  Turok4.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 10/1/15.
 *
 */

#include "Turok4.h"
#include "SOIL/SOIL.h"

namespace Turok4
{
    string _tDir;
    void SetTurokDirectory(const string& TurokDir) { _tDir = TurokDir; }
    string GetTurokDirectory() { return _tDir; }
    string TransformPseudoPathToRealPath(const string& PseudoPath)
    {
        string RealPath;
        if(PseudoPath[0] == 'Y' && PseudoPath[1] == ':') RealPath = PseudoPath.substr(2,PseudoPath.length() - 1);
        else RealPath = PseudoPath;
        
        RealPath = GetTurokDirectory() + RealPath;
        for(int s = 0;s < RealPath.length();s++) { if(RealPath[s] == '\\') { RealPath[s] = '/'; } else { RealPath[s] = tolower(RealPath[s]); } }
        
        size_t extIdx = RealPath.find_last_of(".");
        if(extIdx != string::npos)
        {
            string Ext = RealPath.substr(extIdx,RealPath.length());
            if(Ext == ".bmp"
            || Ext == ".tga")
            {
                RealPath = RealPath.substr(0,extIdx) + ".dds";
            }
        }
        return RealPath;
    }
    string TransformRealPathToPseudoPath(const string& RealPath  )
    {
        return "Not needed yet.";
    }
    
    bool ByteStream::WriteByte(char b)
    {
        if(m_Offset == m_Bytes.size()) { m_Bytes.push_back(b); m_Offset++; }
        else m_Bytes[m_Offset++] = b;
        return true;
    }
    bool ByteStream::WriteInt32(int   i)
    {
        WriteByte(((char*)&i)[0]);
        WriteByte(((char*)&i)[1]);
        WriteByte(((char*)&i)[2]);
        WriteByte(((char*)&i)[3]);
        return true;
    }
    bool ByteStream::WriteInt16(int   i)
    {
        int16_t i16 = i;
        WriteByte(((char*)&i16)[0]);
        WriteByte(((char*)&i16)[1]);
        return true;
    }
    bool ByteStream::WriteFloat(float f)
    {
        WriteByte(((char*)&f)[0]);
        WriteByte(((char*)&f)[1]);
        WriteByte(((char*)&f)[2]);
        WriteByte(((char*)&f)[3]);
        return true;
    }
    bool ByteStream::WriteString(const string& s)
    {
        for(int i = 0;i < s.length();i++) WriteByte(s[i]);
        WriteByte(0);
        return true;
    }
    bool ByteStream::WriteData  (size_t Sz,void* Ptr)
    {
        for(int i = 0;i < Sz;i++) WriteByte(((char*)Ptr)[i]);
        return true;
    }
    char ByteStream::GetByte ()
    {
        if(m_Offset == m_Bytes.size()) return EOF;
        return m_Bytes[m_Offset++];
    }
    int ByteStream::GetInt32()
    {
        int i;
        memcpy(&i,&m_Bytes[m_Offset],4);
        m_Offset += 4;
        return i;
    }
    int16_t ByteStream::GetInt16()
    {
        int16_t i;
        memcpy(&i,&m_Bytes[m_Offset],2);
        m_Offset += 2;
        return i;
    }
    float ByteStream::GetFloat()
    {
        float i;
        memcpy(&i,&m_Bytes[m_Offset],4);
        m_Offset += 4;
        return i;
    }
    string ByteStream::GetString(size_t Len)
    {
        if(Len == 0)
        {
            string s;
            char c = GetByte();
            while(c != 0 && !AtEnd()) { s.push_back(c); c = GetByte(); }
            return s;
        }
        string s;
        for(int i = 0;i < Len;i++) s.push_back(GetByte());
        return s;
    }
    bool ByteStream::GetData(size_t Sz,void* Ptr)
    {
        if(m_Offset + Sz > m_Bytes.size()) return false;
        memcpy(Ptr,&m_Bytes[m_Offset],Sz);
        m_Offset += Sz;
        return true;
    }
    ByteStream* ByteStream::SubData(size_t Sz)
    {
        if(m_Offset + Sz > m_Bytes.size()) return 0;
        ByteStream* s = new ByteStream();
        for(int i = 0;i < Sz;i++) s->WriteByte(GetByte());
        return s;
    }
    /* * * * * * * * * * * * * * * * * * * * * * MTF FILE * * * * * * * * * * * * * * * * * * */
    static string MTFBlockTypeIDStrings[MTFBlock::BT_COUNT] =
    {
        /* Knowns */
        "ELIF",
        "HSEM",
        "EMIT",
        "EPYT",
        "SREV",
        "ETAD",
        "RTXT",
        "LRTM",
        "OFNI",
        "STRV",
        "XDNI",
        "EDON",
        "XOBB",
        "EZIS",
        "ENOB",
        "ECAF",
        "LEKS",
        "SMNM",
        
        /* Unknowns */
        "MOEG",
        "TATS",
        "TSXT",
        "TSNR",
        "HSPV",
        "DISV",
        "LLOC",
        "SIRT",
        "SBUS",
        "OEGA",
        ".reV",
        "tceV",
        "NUOB",
        "TNEC",
        "UOBS",
        "ONCO",
        "ILAF",
        "KNHC",
        "SKNC",
        "TEGT",
        "RDH" ,
        "CBUS",
    };

    bool MTFBlock::Load(FILE *fp)
    {
        char bType[4];
        if(fread(bType,4,1,fp) != 1) return false;
        string TypeString;
        if(bType[0] != 0) TypeString = string(bType,4);
        else TypeString = string(&bType[1],3);
        
        m_Type = BT_COUNT;
        for(int i = 0;i < BT_COUNT;i++)
        {
            if(TypeString == MTFBlockTypeIDStrings[i])
            {
                m_Type = (BLOCK_TYPE)i;
                break;
            }
        }
        
        if(m_Type == BT_COUNT)
        {
            //cout << "Unexpected type string: \"" << TypeString << "\".\n";
        }
        
        if(fread(&m_Unk0,sizeof(int),1,fp) != 1) return false;
        if(fread(&m_Unk1,sizeof(int),1,fp) != 1) return false;
        if(fread(&m_Unk2,sizeof(int),1,fp) != 1) return false;
        if(fread(&m_DataOffset,sizeof(int),1,fp) != 1) return false;
        if(fread(&m_DataSize  ,sizeof(int),1,fp) != 1) return false;
        
        return true;
    }
    string MTFBlock::GetTypeString() const
    {
        return MTFBlockTypeIDStrings[m_Type];
    }

    size_t SubMesh::GetVertexCount() const
    {
        switch(m_vType)
        {
            case -1: return m_VerticesC.size();
            case  0: return m_VerticesA.size();
            case  3: return m_VerticesB.size();
            default: { return 0; }
        }
        return 0;
    }
    void SubMesh::GetVertex  (size_t Idx,float* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].x,sizeof(float) * 3); return; }
            case  0: { memcpy(Ptr,&m_VerticesA[Idx].x,sizeof(float) * 3); return; }
            case  3: { memcpy(Ptr,&m_VerticesB[Idx].x,sizeof(float) * 3); return; }
            default: { return; }
        }
    }
    void SubMesh::GetNormal  (size_t Idx,float* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].nx,sizeof(float) * 3); return; }
            case  0: { memcpy(Ptr,&m_VerticesA[Idx].nx,sizeof(float) * 3); return; }
            case  3: { memcpy(Ptr,&m_VerticesB[Idx].nx,sizeof(float) * 3); return; }
            default: { return; }
        }
    }
    void SubMesh::GetTexCoord(size_t Idx,float* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].u,sizeof(float) * 2); return; }
            case  0: { memcpy(Ptr,&m_VerticesA[Idx].u,sizeof(float) * 2); return; }
            case  3: { memcpy(Ptr,&m_VerticesB[Idx].u,sizeof(float) * 2); return; }
            default: { return; }
        }
    }
    void SubMesh::GetUnk0    (size_t Idx,char * Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].Unk0,6); return; }
            case  0: { memcpy(Ptr,&m_VerticesA[Idx].Unk0,4); return; }
            case  3: { memcpy(Ptr,&m_VerticesB[Idx].Unk0,4); return; }
            default: { return; }
        }
    }
    void SubMesh::GetUnk1    (size_t Idx,float* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].Unk1,sizeof(float) * 1); return; }
            case  3: { memcpy(Ptr,&m_VerticesB[Idx].Unk1,sizeof(float) * 2); return; }
            default: { return; }
        }
    }
    void SubMesh::GetUnk2    (size_t Idx,char* Ptr) const
    {
        switch(m_vType)
        {
            case -1: { memcpy(Ptr,&m_VerticesC[Idx].Unk2,8); return; }
            default: { return; }
        }
    }

    ActorMesh::ActorMesh() : m_IsAnimated(false)
    {
    }
    ActorMesh::~ActorMesh()
    {
        for(int i = 0;i < m_Blocks.size();i++) delete m_Blocks[i];
        m_Blocks.clear();
    }
    bool ActorMesh::Load(const string &File)
    {
        FILE* fp = fopen(File.c_str(),"rb");
        if(!fp) return false;
        
        for(int i = 0;i < File.length();i++)
        {
            if(File[i] == '.')
            {
                string ext = File.substr(i,File.length() - 1);
                if((ext[1] == 'a'
                 || ext[1] == 'A')
                 &&(ext[2] == 't'
                 || ext[2] == 'T')
                 &&(ext[3] == 'f'
                 || ext[3] == 'F'))
                
                {
                    m_IsAnimated = true;
                    break;
                }
            }
        }
        
        int BlockCount;
        if(fread(&BlockCount,sizeof(int),1,fp) != 1) { fclose(fp); return false; }
        
        for(int i = 0;i < BlockCount;i++)
        {
            MTFBlock* b = new MTFBlock();
            if(!b->Load(fp)) { delete b; fclose(fp); return false; }
            m_Blocks.push_back(b);
        }
        
        for(int i = 0;i < m_Blocks.size();i++)
        {
            //printf("Block[%4d]: %4s %6d %6d %6d %6d %6d\n",i,m_Blocks[i]->GetTypeString().c_str()
            //                                               ,m_Blocks[i]->m_Unk0
            //                                               ,m_Blocks[i]->m_Unk1
            //                                               ,m_Blocks[i]->m_Unk2
            //                                               ,m_Blocks[i]->m_DataSize
            //                                               ,m_Blocks[i]->m_DataOffset);
            
            if(m_Blocks[i]->m_DataOffset == 0 || m_Blocks[i]->m_DataSize == -1) continue;
            fseek(fp,m_Blocks[i]->m_DataOffset,SEEK_SET);
            
            m_Blocks[i]->m_Data = new char[m_Blocks[i]->m_DataSize];
            if(fread(m_Blocks[i]->m_Data,m_Blocks[i]->m_DataSize,1,fp) != 1)
            {
                delete [] m_Blocks[i]->m_Data;
                fclose(fp);
                return false;
            }
            
            if(m_Blocks[i]->GetType() == MTFBlock::BT_SKELETON) m_IsAnimated = true;
        }
        for(int i = 0;i < m_Blocks.size();i++)
        {
            ProcessBlock(i);
        }
        
        fclose(fp);
        return true;
    }
    void ActorMesh::ProcessBlock(int Idx)
    {
        MTFBlock* b = m_Blocks[Idx];
        switch(b->GetType())
        {
            case MTFBlock::BT_TIME:
            {
                //printf("Block[%4d]: ",Idx);
                int Tm = *(int*)b->m_Data;
                //printf("Time(?): %d.\n",Tm);
                break;
            }
            case MTFBlock::BT_INFO:
            {
                //printf("Block[%4d]: ",Idx);
                int *Info = (int*)b->m_Data;
                //printf("Info(?): %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d.\n",Info[0] ,Info[1] ,Info[2] ,Info[3] ,Info[4]
                //                                                            ,Info[5] ,Info[6] ,Info[7] ,Info[8] ,Info[9]);
                
                MeshInfo m;
                memcpy(&m,Info,10 * sizeof(int));
                m_MeshInfos.push_back(m);
                break;
            }
            case MTFBlock::BT_SUBMESHNAMES:
            {
                int dPos = 0;
                int NameCount = *(int*)&b->m_Data[dPos]; dPos += sizeof(int);
                
                for(int i = 0;i < NameCount;i++)
                {
                    int NameLen = *(int*)&b->m_Data[dPos]; dPos += sizeof(int);
                    string Name;
                    for(int s = 0;s < NameLen;s++)
                    {
                        Name += b->m_Data[dPos++];
                    }
                    m_SubMeshNames.push_back(Name);
                }
                
                break;
            }
            case MTFBlock::BT_TYPE:
            {
                //printf("Block[%4d]: ",Idx);
                int Type = *(int*)b->m_Data;
                //printf("Type(?): %d.\n",Type);
                break;
            }
            case MTFBlock::BT_VERSION:
            {
                //printf("Block[%4d]: ",Idx);
                int Version = *(int*)b->m_Data;
                //printf("Version(?): %d.\n",Version);
                break;
            }
            case MTFBlock::BT_DATE:
            {
                //printf("Block[%4d]: ",Idx);
                char* Date = b->m_Data;
                //printf("Creation Date: %s\n",Date);
                break;
            }
            case MTFBlock::BT_TEXTURE:
            {
                //printf("Block[%4d]: ",Idx);
                char* Texture = b->m_Data;
                
                string ActualFilename = TransformPseudoPathToRealPath(string(Texture,b->m_DataSize));
                
                //printf("Texture: %s\n",ActualFilename.c_str());
                
                m_Textures.push_back(ActualFilename);
                
                break;
            }
            case MTFBlock::BT_RNST:
            {
                //printf("Block[%4d]: ",Idx);
                int* RNST = (int*)b->m_Data;
                //printf("RNST: %6d %6d %6d %6d %6d %6d\n",RNST[0],RNST[1],RNST[2],RNST[3],RNST[4],RNST[5]);
                if(b->m_DataSize != sizeof(TSNR))
                {
                    //printf("Warning: Data block size != sizeof(TSNR).\n");
                }
                
                TSNR t;
                memcpy(&t,RNST,sizeof(TSNR));
                m_TSNRs.push_back(t);
                break;
            }
            case MTFBlock::BT_TXST:
            {
                //printf("Block[%4d]: ",Idx);
                TXST t;
                memcpy(&t,b->m_Data,b->m_DataSize);
                //printf("TXST(?): %4d %4d %4d %4d %f %4d %4d %4d %4d %f %f %4d %4d %4d %4d %4d %f %4d %f %d\n",t.Unk0 ,t.TextureID,t.Unk1 ,t.Unk2
                //                                                                                             ,t.Unk3 ,t.Unk4     ,t.Unk5 ,t.Unk6
                //                                                                                             ,t.Unk7 ,t.Unk8     ,t.Unk9 ,t.Unk10
                //                                                                                             ,t.Unk11,t.Unk12    ,t.Unk13,t.Unk14
                //                                                                                             ,t.Unk15,t.Unk16    ,t.Unk17,t.Unk18);
                       
                m_TXSTs.push_back(t);
            }
            case MTFBlock::BT_SUBS:
            {
                //printf("Block[%4d]: ",Idx);
                int Version = *(int*)b->m_Data;
                //printf("SBUS(?): %d.\n",Version);
                break;
            }
            case MTFBlock::BT_VSID:
            {
                //printf("Block[%4d]: ",Idx);
                int DISV = *(int*)b->m_Data;
                //printf("DISV(?): %d\n",DISV);
                break;
            }
            case MTFBlock::BT_MATERIAL:
            {
                //printf("Block[%4d]: ",Idx);
                if(b->m_DataSize == 20)
                {
                    int Unk0 = *(int*)&b->m_Data[0];
                    float Unk1 = *(float*)&b->m_Data[4];
                    int Unk2 = *(int*)&b->m_Data[8];
                    int Unk3 = *(int*)&b->m_Data[12];
                    int Unk4 = *(int*)&b->m_Data[16];
                
                    //printf("Material(?): %4d %4.5f %4d %4d %4d.\n",Unk0,Unk1,Unk2,Unk3,Unk4);
                    
                    MTRL m;
                    memcpy(&m,b->m_Data,20);
                    m_MTRLs.push_back(m);
                }
                else if(b->m_DataSize == 4)
                {
                    float Unk0 = *(float*)&b->m_Data[0];
                
                    //printf("Material(?): %f.\n",Unk0);
                }
                //else printf("(Null material)\n");
                break;
            }
            case MTFBlock::BT_BOUNDINGBOX:
            {
                //printf("Block[%4d]: ",Idx);
                float* Bounds = (float*)b->m_Data;
                //printf("Bounding Box: (%4.5f, %4.5f, %4.5f) (%4.5f, %4.5f, %4.5f).\n",Bounds[0],Bounds[1],Bounds[2],
                //                                                                      Bounds[3],Bounds[4],Bounds[5]);
                break;
            }
            case MTFBlock::BT_VERTICES:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = new SubMesh();
                if(m_IsAnimated) m->m_vType = -1;
                else m->m_vType = m_MeshInfos[m_MeshInfos.size() - 1].Unk1;
                
                int dPos = 0;
                switch(m->m_vType)
                {
                    case -1:
                    {
                        int vCount = *(int*)b->m_Data; dPos += sizeof(int);
                        //printf("SubMesh (V-Type C): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            ATFVertexTypeA v;
                            memcpy(&v,&b->m_Data[dPos],sizeof(ATFVertexTypeA));
                            dPos += sizeof(ATFVertexTypeA);
                            
                            //printf("v: %5.5f,%5.5f,%5.5f %5.5f,%5.5f,%5.5f %5.5f,%5.5f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);
                            
                            m->AddVertex(v);
                        }
                        break;
                    }
                    case 0:
                    {
                        int vCount = b->m_DataSize / sizeof(MTFVertexTypeA);
                        //printf("SubMesh (V-Type A): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            MTFVertexTypeA v;
                            memcpy(&v,&b->m_Data[dPos],sizeof(MTFVertexTypeA));
                            dPos += sizeof(MTFVertexTypeA);
                            
                            //printf("v: %f,%f,%f %f,%f,%f %f,%f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);
                            
                            m->AddVertex(v);
                        }
                        break;
                    }
                    case 3:
                    {
                        int vCount = b->m_DataSize / sizeof(MTFVertexTypeB);
                        //printf("SubMesh (V-Type B): %lu VC: %d.\n",m_SubMeshes.size() + 1,vCount);
                        for(int i = 0;i < vCount;i++)
                        {
                            MTFVertexTypeB v;
                            memcpy(&v,&b->m_Data[dPos],sizeof(MTFVertexTypeB));
                            dPos += sizeof(MTFVertexTypeB);
                            
                            //printf("v: %f,%f,%f %f,%f,%f %f,%f\n",v.x,v.y,v.z,v.nx,v.ny,v.nz,v.u,v.v);
                            
                            m->AddVertex(v);
                        }
                        break;
                    }
                    default:
                    {
                        //printf("Encountered unknown vertex type! Study me!\n");
                    }
                }
                m_SubMeshes.push_back(m);
                break;
            }
            case MTFBlock::BT_INDICES:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = m_SubMeshes[m_SubMeshes.size() - 1];
                int iCount = b->m_DataSize / sizeof(int16_t);
                //printf("SubMesh: %lu IC: %d.\n",m_SubMeshes.size(),iCount);
                int dPos = 0;
                for(int i = 0;i < iCount;i++)
                {
                    int16_t Idx = 0;
                    memcpy(&Idx,&b->m_Data[dPos],sizeof(int16_t));
                    dPos += sizeof(int16_t);
                    
                    //printf("i: %d\n",Idx);
                    
                    m->AddIndex(Idx);
                }
                break;
            }
            case MTFBlock::BT_SUBC:
            {
                //printf("Block[%4d]: ",Idx);
                SubMesh* m = m_SubMeshes[m_SubMeshes.size() - 1];
                MeshChunk* Chunk = new MeshChunk(m);
                
                int dPos = 0;
                dPos += 4; //There always seems to be two null indices at the start of every index list too
                while(dPos != b->m_DataSize)
                {
                    int16_t Idx = 0;
                    memcpy(&Idx,&b->m_Data[dPos],sizeof(int16_t));
                    dPos += sizeof(int16_t);
                    if((uint16_t)Idx == 0xFFFF)
                    {
                        //Last two indices always null for some reason
                        Chunk->Remove(Chunk->GetIndexCount() - 1);
                        Chunk->Remove(Chunk->GetIndexCount() - 1);
                        
                        //m->AddChunk(Chunk);
                        
                        //Chunk = new MeshChunk(m);
                        dPos += 2; //There's always a second 0xFFFF
                        dPos += 4; //There always seems to be two null indices at the start of every index list too
                        continue;
                    }
                    
                    Chunk->AddIndex(Idx);
                }
                //Last two indices always null for some reason
                Chunk->Remove(Chunk->GetIndexCount() - 1);
                Chunk->Remove(Chunk->GetIndexCount() - 1);
                m->AddChunk(Chunk);
                
                //printf("%zu Sections in chunk.\n",m->GetChunkCount());
                break;
            }
            case MTFBlock::BT_FACE:
            {
                //printf("\n");
                break;
            }
            default:
            {
                //printf("Unknown.\n");
                break;
            }
        }
    }
    
    /* * * * * * * * * * * * * * * * * * * * * * ATR FILE * * * * * * * * * * * * * * * * * * */
    
    static string BlockTypeIDs[BT_COUNT] =
    {
        "ACTOR",
        "PATH",
        "NAVDATA",
        
        "ID",
        "POS",
        "ROT",
        "SCALE",
        "NAME",
        "ACTOR_VARIABLES",
        "LINK",
        "CAUSE",
        "EVENT",
        "PATHID",
        "ACTOR_CODE",
        "ACTOR_MESH_AXIS",
        "ACTOR_MESH_BOUNDS",
        "ACTOR_PROPERTIES",
        "LINK_LISTS",
        "ACTOR_TEXTURE_SET",
        "GRND_Y",
        "MODES",
        
        "ACTOR_MESH",
        "ACTORINSTANCES",
        "PRECACHEFILE",
        
        "AFFECTSTARGET",
        "TIMETOREACHMAXSTRENGTH",
        "TIMETOTURNOFF",
        "HIDE",
        "HOLD",
        "GAGROUPNAME",
        "GAMINNUMBERS",
        "GAGROUPATTACK",
        "SXZANGLE",
        "SOUNDRADIUS",
        "CLOSERANGEDIST",
        "MEDIUMRANGEDIST",
        "AU_A",
        "AU_B",
        "AU_C",
        "AU_D",
        "HEALTH",
        "MAXHEALTH",
        "TURNON",
        "TYPE",
        "COUNTERS",
        "ONLYPTARGET",
        "SPAWNACTOR1CHANCE",
        "CLOSERANGECHANCE",
        "IGNOREPLAYER",
        "PROVOKEONLY",
        "FDMULT",
        "COLLIDES",
        "IGNORES",
        "TOUCHES",
        "LIGHTCOLOR",
        "LIGHTINTENSITY",
        "FRICTION",
        "SPINX",
        "SPINY",
        "SPINZ",
        "A_NDD",
        "A_MDD",
        "A_SON",
        "A_FL",
        "C_B",
        "F_B",
        "F_C",
        "F_F",
        "F_T",
        "F_ED",
        "SMTYPE",
        "LEASHRADIUS",
        "LDSOUND",
        "MDSOUND",
        "HDSOUND",
        "SIGHTRADIUS",
        "SYANGLE",
        "ATTACKRESETTIME",
        "USEHEADTRACKING",
        "INITIALSTATE",
        "TURNINGSPEED",
        "FLAPBEHAVIOR",
        "UPGRADE1SLOT0",
        "UPGRADE2SLOT0",
        "WEAPONSLOT0",
        "WEAPONSLOT1",
        "WEAPONSLOT2",
        "WEAPONSLOT3",
        "WEAPONSLOT4",
        "WEAPONSLOT5",
        "WEAPONSLOT6",
        "WEAPONSLOT7",
        "STARTSOFFWITHWEAPON0",
        "STARTSOFFWITHWEAPON1",
        "STARTSOFFWITHWEAPON2",
        "STARTSOFFWITHWEAPON3",
        "STARTSOFFWITHWEAPON4",
        "STARTSOFFWITHWEAPON5",
        "STARTSOFFWITHWEAPON6",
        "STARTSOFFWITHWEAPON7",
        "ALWAYSRUN",
        "DEFTRANSBLEND",
        
        "DUMMIES",
        "DEFT",
        "COLS",
        "VERSION",
        "HOTPS",
        "TRNS",
        "LINK",
        "LINKS",
    };
    
    BLOCK_TYPE GetBlockTypeFromID(const string& ID)
    {
        for(int i = 0;i < BT_COUNT;i++)
        {
            if(BlockTypeIDs[i] == ID) return (BLOCK_TYPE)i;
        }
        //printf("Encountered new block type (%s).\n",ID.c_str());
        return BT_COUNT;
    }
    
    
    ActorVariables::ActorVariables()
    {
    }
    ActorVariables::~ActorVariables()
    {
    }

    bool ActorVariables::Load(ByteStream* Data)
    {
        Data->SetOffset(0);
        while(!Data->AtEnd(1))
        {
            Block* b = new Block();
            if(!b->Load(Data)) { delete b; return false; }
            m_Blocks.push_back(b);
        }
        
        ProcessBlocks();
        
        return true;
    }
    bool ActorVariables::Save(ByteStream* Data)
    {
		return false;
    }
    void ActorVariables::ProcessBlocks()
    {
        for(int i = 0;i < m_Blocks.size();i++)
        {
            Block* b = m_Blocks[i];
            ByteStream* d = b->GetData();
            d->SetOffset(0);
            switch(b->GetType())
            {
                case BT_AFFECTS_TARGET:
                {
                    break;
                }
                case BT_TIME_TO_REACH_MAX_STRENGTH:
                {
                    break;
                }
                case BT_TIME_TO_TURN_OFF:
                {
                    break;
                }
                case BT_HIDE:
                {
                    break;
                }
                case BT_GA_GROUP_NAME:
                {
                    break;
                }
                case BT_GA_MIN_NUMBERS:
                {
                    break;
                }
                case BT_GA_GROUP_ATTACK:
                {
                    break;
                }
                case BT_SXZ_ANGLE:
                {
                    break;
                }
                case BT_SOUND_RADIUS:
                {
                    break;
                }
                case BT_CLOSE_RANGE_DIST:
                {
                    break;
                }
                case BT_AU_A:
                {
                    break;
                }
                case BT_AU_B:
                {
                    break;
                }
                case BT_AU_C:
                {
                    break;
                }
                case BT_AU_D:
                {
                    break;
                }
                case BT_HEALTH:
                {
                    break;
                }
                case BT_MAX_HEALTH:
                {
                    break;
                }
                case BT_TURN_ON:
                {
                    break;
                }
                case BT_TYPE:
                {
                    break;
                }
                case BT_COUNTERS:
                {
                    break;
                }
                case BT_ONLY_P_TARGET:
                {
                    break;
                }
                case BT_SPAWN_ACTOR1_CHANCE:
                {
                    break;
                }
                case BT_IGNORE_PLAYER:
                {
                    break;
                }
                case BT_PROVOKE_ONLY:
                {
                    break;
                }
                case BT_FD_MULT:
                {
                    break;
                }
                case BT_COLLIDES:
                {
                    break;
                }
                case BT_IGNORES:
                {
                    break;
                }
                case BT_TOUCHES:
                {
                    break;
                }
                case BT_LIGHT_COLOR:
                {
                    break;
                }
                case BT_LIGHT_INTENSITY:
                {
                    break;
                }
                case BT_FRICTION:
                {
                    break;
                }
                case BT_SPIN_X:
                {
                    Spin.x = *((float*)d->Ptr());
                    break;
                }
                case BT_SPIN_Y:
                {
                    Spin.y = *((float*)d->Ptr());
                    break;
                }
                case BT_SPIN_Z:
                {
                    Spin.z = *((float*)d->Ptr());
                    break;
                }
                case BT_A_NDD:
                {
                    break;
                }
                case BT_A_MDD:
                {
                    break;
                }
                case BT_A_SON:
                {
                    break;
                }
                case BT_A_FL:
                {
                    break;
                }
                case BT_SM_TYPE:
                {
                    break;
                }
                case BT_LEASH_RADIUS:
                {
                    break;
                }
                case BT_LD_SOUND:
                {
                    break;
                }
                case BT_MD_SOUND:
                {
                    break;
                }
                case BT_HD_SOUND:
                {
                    break;
                }
                default:
                {
                    //printf("Unsupported actor variable (%s).\n",m_Blocks[i]->GetTypeString().c_str());
                }
            }
            if(m_Blocks[i]->GetType() == BT_COUNT)
            {
                //printf("Unsupported actor variable (%s).\n",m_Blocks[i]->GetTypeString().c_str());
            }
        }
    }
    
    Actor::~Actor()
    {
        if(m_Mesh) delete m_Mesh;
        //if(m_File) delete m_File;
    }
    string Actor::GetFilename() const
    {
        if(!m_File) return "";
        return m_File->m_File;
    }

    ATRFile::ATRFile() : m_Actor(0), m_Data(0), m_Root(0)
    {
    }

    ATRFile::~ATRFile()
    {
    }

    bool ATRFile::Load(const string& Filename)
    {
        //Load file
        FILE* fp = fopen(Filename.c_str(),"rb");
        if(!fp) { printf("Unable to open file.\n"); return false; }
        m_Data = new ByteStream(fp);
        fclose(fp);
        
        m_Actor = new Actor(this);
        
        if(!CheckHeader()) return false;
        m_Root = new Block();
        m_Root->Load(m_Data);
        
        unsigned char PathLen = m_Root->GetData()->GetByte();
        m_File = m_Root->GetData()->GetString(PathLen);
        m_Root->GetData()->Offset(1);
        
        //printf("ATR: %s.\n",Filename.c_str());
        while(!m_Root->GetData()->AtEnd(1))
        {
            Block* b = new Block();
            b->Load(m_Root->GetData());
            m_Root->AddChildBlock(b);
        }
        
        ProcessBlocks();
        
        return true;
    }
    void ATRFile::ProcessBlocks()
    {
        for(int i = 0;i < m_Root->GetChildCount();i++)
        {
            Block* b = m_Root->GetChild(i);
            ByteStream* Data = b->GetData();
            Data->SetOffset(0);
            switch(b->GetType())
            {
                case BT_ACTOR_MESH:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_ActorMeshFile = Data->GetString();
                    
                    ActorMesh* m = new ActorMesh();
                    m->Load(TransformPseudoPathToRealPath(m_ActorMeshFile));
                    m_Actor->m_Mesh = m;
                    
                    break;
                }
                case BT_ACTOR_INSTANCES:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_InstancesFile = Data->GetString();
                    
                    ATIFile* ATI = new ATIFile();
                    ATI->Load(TransformPseudoPathToRealPath(m_InstancesFile));
                    m_ActorInstanceFiles.push_back(ATI);
                    
                    break;
                }
                case BT_VERSION:
                {
                    m_Version = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_CODE:
                {
                    m_ActorCode = Data->GetString();
                    break;
                }
                case BT_ACTOR_MESH_AXIS:
                {
                    m_ActorMeshAxis.x = Data->GetFloat();
					m_ActorMeshAxis.y = Data->GetFloat();
					m_ActorMeshAxis.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_PRECACHE_FILE:
                {
                    Data->GetByte(); //Path length (not needed)
                    m_PrecacheFile = Data->GetString();
                    //printf("Precache files not yet understood.\n");
                    
                    break;
                }
                case BT_ACTOR_VARIABLES  :
                case BT_ACTOR_MESH_BOUNDS:
                case BT_ACTOR_TEXTURE_SET:
                case BT_ACTOR_PROPERTIES :
                case BT_ACTOR_POSITION   :
                case BT_ACTOR_ROTATION   :
                case BT_ACTOR_SCALE      :
                case BT_DUMMIES          :
                case BT_GRND_Y           :
                case BT_DEFT             :
                case BT_COLS             :
                case BT_MODES            :
                case BT_LINK_LISTS       :
                case BT_LINK             :
                case BT_LINKS            :
                case BT_TRNS             :
                case BT_HOTPS            : { break; }
                default:
                {
                    //printf("Unsupported ATR block type (%s).\n",BlockTypeIDs[b->GetType()].c_str());
                    break;
                }
            }
        }
    }
    bool ATRFile::Save(const string &Filename)
    {
        ByteStream* Data = new ByteStream();
        Data->WriteData(4,m_Hdr);
        m_Root->Save(Data);
        return true;
    }

    bool ATRFile::CheckHeader()
    {
        char m_Hdr[4];
        if(!m_Data->GetData(4,m_Hdr))
        {
            printf("Unable to read header. (Empty file?)\n");
            return false;
        }
        
        if(m_Hdr[1] != 'a'
        || m_Hdr[2] != 't'
        || m_Hdr[3] != 'r')
        {
            printf("Invalid ATR header.\n");
            return false;
        }
        
        return true;
    }
    
    Block::Block()
    {
    }
    Block::~Block()
    {
    }
    bool Block::Load(ByteStream* Data)
    {
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
        m_PreBlockFlag = Data->GetByte();
        size_t Size = 0;
        
        switch(m_PreBlockFlag)
        {
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4F:
            {
                //Hdr[0] = Unknown (when not part of 2-byte block size)
                //Hdr[1] = Block size in bytes (starts after block ID, including the null)
                //Hdr[2] = Block ID string length
                
                //Read block header
                Data->GetData(3,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)(unsigned char)m_Hdr[1];
                break;
            }
            case 0x81: { }
            case 0x82:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Last  8 bits of 16 bit integer representing block size
                //Hdr[3] = Block ID string length
                
                //Read block header
                Data->GetData(4,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)*((uint16_t*)&m_Hdr[1]);
                break;
            }
            case 0x61:
            case 0x6C:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Unknown
                //Hdr[3] = Unknown
                //Hdr[4] = Block ID string length
                
                //Read block header
                Data->GetData(5,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)(unsigned char)m_Hdr[1];
                
                break;
            }
            case 0xA1:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First 8 bits of 16 bit integer representing block size
                //Hdr[2] = Last  8 bits of 16 bit integer representing block size
                //Hdr[3] = Unknown
                //Hdr[4] = Unknown
                //Hdr[5] = Block ID string length
                
                //Read block header
                Data->GetData(6,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)*((uint16_t*)&m_Hdr[1]);
                
                break;
            }
            case 0xC2:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First  8 bits of 32 bit integer representing block size
                //Hdr[2] = Second 8 bits of 32 bit integer representing block size
                //Hdr[3] = Third  8 bits of 32 bit integer representing block size
                //Hdr[4] = Last   8 bits of 32 bit integer representing block size
                //Hdr[5] = Block ID string length
                
                //Read block header
                Data->GetData(6,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)*((uint32_t*)&m_Hdr[1]);
                break;
            }
            case 0xE1:
            {
                //Hdr[0] = Unknown
                //Hdr[1] = First  8 bits of 32 bit integer representing block size
                //Hdr[2] = Second 8 bits of 32 bit integer representing block size
                //Hdr[3] = Third  8 bits of 32 bit integer representing block size
                //Hdr[4] = Last   8 bits of 32 bit integer representing block size
                //Hdr[5] = Unknown
                //Hdr[6] = Unknown
                //Hdr[7] = Block ID string length
                
                //Read block header
                Data->GetData(8,m_Hdr);
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                m_BlockID = Data->GetString();
                
                //Establish block size
                Size = (size_t)*((uint32_t*)&m_Hdr[1]);
                break;
            }
            default  :
            {
                printf("Unknown block type 0x%2X.\n",m_PreBlockFlag);
            }
        }
        
        //printf("0x%2X | %10zu | %16s\n",m_PreBlockFlag,Size,m_BlockID.c_str());
        m_Type = GetBlockTypeFromID(m_BlockID);
        
        m_Data = Data->SubData(Size);
        if(m_Data) m_Data->SetOffset(0);
        return true;
    }
    bool Block::Save(ByteStream* Data)
    {
        bool Has16BitBlockSize = false;
        bool HasUnkBlockHeaderType = false;
        int UnkBlockHeaderType = 0;
        
        switch(m_PreBlockFlag)
        {
            case 0x82: { Data->WriteByte(m_PreBlockFlag); Has16BitBlockSize     = true; break; }
            case 0x81: { Data->WriteByte(m_PreBlockFlag); Has16BitBlockSize     = true; break; }
            case 0x61: { Data->WriteByte(m_PreBlockFlag); HasUnkBlockHeaderType = true; UnkBlockHeaderType = 0; break; }
            case 0xA1: { Data->WriteByte(m_PreBlockFlag); HasUnkBlockHeaderType = true; UnkBlockHeaderType = 1; break; }
            case 0xC2: { Data->WriteByte(m_PreBlockFlag); HasUnkBlockHeaderType = true; UnkBlockHeaderType = 2; break; }
            case 0xE1: { Data->WriteByte(m_PreBlockFlag); HasUnkBlockHeaderType = true; UnkBlockHeaderType = 3; break; }
        }
        
        if(!HasUnkBlockHeaderType)
        {
            if(!Data->WriteData(4,m_Hdr)) return false;
        }
        else if(HasUnkBlockHeaderType)
        {
            switch(UnkBlockHeaderType)
            {
                case 0:
                {
                    if(!Data->WriteData(5,m_Hdr)) return false;
                    break;
                }
                case 1:
                {
                    if(!Data->WriteData(6,m_Hdr)) return false;
                    break;
                }
                case 2:
                {
                    if(!Data->WriteData(6,m_Hdr)) return false;
                    break;
                }
                case 3:
                {
                    if(!Data->WriteData(8,m_Hdr)) return false;
                    break;
                }
            }
        }
        
        if(!Data->WriteString(m_BlockID)) return false;
        if(m_Type == BT_ACTOR)
        {
            m_Data->SetOffset(0);
            Data->WriteString(m_Data->GetString());
        }
        
        
        if(m_Children.size() > 0)
        {
            for(int c = 0;c < m_Children.size();c++) m_Children[c]->Save(Data);
        }
		else if(m_Data->GetSize() > 0)
        {
            m_Data->SetOffset(0);
            if(!Data->WriteData(m_Data->GetSize(),m_Data->Ptr())) return false;
        }
        return true;
    }
    
    ATIFile::ATIFile()
    {
    }
    ATIFile::~ATIFile()
    {
        for(int i = 0;i < m_Blocks.size();i++) delete m_Blocks[i];
        for(int i = 0;i < m_Actors.size();i++)
        {
            delete m_Actors[i]->Mesh;
            delete m_Actors[i]->Actor;
            delete m_Actors[i];
        }
    }

    bool ATIFile::Load(const string& File)
    {
        FILE* fp = fopen(File.c_str(),"rb");
        if(!fp) return false;
        ByteStream* Data = new ByteStream(fp);
        fclose(fp);
        
        if(!Data->GetData(4,m_Hdr)) { printf("Unable to read header.\n"); delete Data; return false; }
        
        if(m_Hdr[1] != 'a'
        || m_Hdr[2] != 't'
        || m_Hdr[3] != 'i')
        {
            printf("Incorrect header, not actor instances file.\n");
            delete Data;
            return false;
        }
        
        while(!Data->AtEnd(1))
        {
            Block* b = new Block();
            b->Load(Data);
            m_Blocks.push_back(b);
        }
        
        ProcessBlocks();
        
        delete Data;
        m_File = File;
        return true;
    }
    bool ATIFile::Save(const string& File)
    {
        //Update actor properties
        for(int i = 0;i < m_Actors.size();i++)
        {
            SaveActorBlock(m_Actors[i]->BlockIdx);
        }
        
        ByteStream* Data = new ByteStream();
        Data->WriteData(4,m_Hdr);
        
        for(int i = 0;i < m_Blocks.size();i++)
        {
            m_Blocks[i]->Save(Data);
        }
        
        FILE* fp = fopen(File.c_str(),"wb");
        if(!fp) { printf("Unable to open file <%s> for writing.\n",File.c_str()); delete Data; return false; }
        Data->SetOffset(0);
        if(fwrite(Data->Ptr(),Data->GetSize(),1,fp) != 1)
        {
            fclose(fp);
            delete Data;
            printf("Unable to write %zu bytes.\n",Data->GetSize());
            return false;
        }
        fclose(fp);
        return true;
    }
    void ATIFile::ProcessBlocks()
    {
        for(int i = 0;i < m_Blocks.size();i++)
        {
            switch(m_Blocks[i]->GetType())
            {
                case BT_ACTOR           : { ProcessActorBlock(i); break; }
                case BT_PATH            : { break; }
                case BT_NAVDATA         : { break; }
                case BT_ACTOR_VARIABLES : { break; }
                case BT_VERSION         : { break; }
                default:
                {
                    //printf("Unsupported ATI block type (%s).\n",BlockTypeIDs[m_Blocks[i]->GetType()].c_str());
                    break;
                }
            }
        }
    }
    void ATIFile::ProcessActorBlock(int Idx)
    {
        Block* b = m_Blocks[Idx];
        b->GetData()->SetOffset(0);
        unsigned char PathLen = b->GetData()->GetByte();
        string Path = b->GetData()->GetString(PathLen);
        b->GetData()->Offset(1);
        
        while(!b->GetData()->AtEnd(1))
        {
            Block* aBlock = new Block();
            aBlock->Load(b->GetData());
            b->AddChildBlock(aBlock);
        }
        
        ActorDef* d = new ActorDef();
        d->ActorFile = Path;
        d->BlockIdx  = Idx ;
        d->Parent    = this;
        d->PathID = d->ID = -1;
        d->Actor = new ATRFile();
        if(!d->Actor->Load(TransformPseudoPathToRealPath(Path)))
        {
            delete d->Actor;
            return;
        }
        
        for(int i = 0;i < b->GetChildCount();i++)
        {
            Block* cBlock = b->GetChild(i);
            ByteStream* Data = cBlock->GetData();
            
            switch(cBlock->GetType())
            {
                case BT_ACTOR_POSITION:
                {
                    d->Position.x = Data->GetFloat();
                    d->Position.y = Data->GetFloat();
                    d->Position.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_ROTATION:
                {
                    d->Rotation.x = Data->GetFloat();
                    d->Rotation.y = Data->GetFloat();
                    d->Rotation.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_SCALE   :
                {
                    d->Scale.x = Data->GetFloat();
                    d->Scale.y = Data->GetFloat();
                    d->Scale.z = Data->GetFloat();
                    break;
                }
                case BT_ACTOR_NAME    :
                {
                    d->Name = Data->GetString();
                    break;
                }
                case BT_ACTOR_ID      :
                {
                    d->ID = (unsigned char)Data->GetByte();
                    break;
                }
                case BT_ACTOR_PATH_ID :
                {
                    d->PathID = (unsigned char)Data->GetByte();
                    break;
                }
                case BT_ACTOR_VARIABLES:
                {
                    ActorVariables* v = new ActorVariables();
                    if(!v->Load(Data)) { printf("Unable to load actor variables.\n"); }
                    d->Actor->GetActor()->SetActorVariables(v);
                    break;
                }
                case BT_ACTOR_LINK:
                {
                    break;
                }
                default:
                {
                    //printf("Unsupported actor block type (%s).\n",BlockTypeIDs[cBlock->GetType()].c_str());
                }
            }
        }
        
        m_Actors.push_back(d);
        d->Actor->GetActor()->m_Def = d;
    }
    void ATIFile::SaveActorBlock(int Idx)
    {
        Block* b = m_Blocks[Idx];
        b->GetData()->SetOffset(0);
        unsigned char PathLen = b->GetData()->GetByte();
        string Path = b->GetData()->GetString(PathLen);
        b->GetData()->Offset(1);
        ActorDef* d = m_Actors[Idx];
        
        for(int i = 0;i < b->GetChildCount();i++)
        {
            Block* cBlock = b->GetChild(i);
            ByteStream* Data = cBlock->GetData();
            Data->SetOffset(0);
            
            switch(cBlock->GetType())
            {
                case BT_ACTOR_POSITION:
                {
                    Data->WriteData(sizeof(ActorVec3),&d->Position.x);
                    break;
                }
                case BT_ACTOR_ROTATION:
                {
                    Data->WriteData(sizeof(ActorVec3),&d->Rotation.x);
                    break;
                }
                case BT_ACTOR_SCALE   :
                {
                    Data->WriteData(sizeof(ActorVec3),&d->Scale   .x);
                    break;
                }
                case BT_ACTOR_NAME    :
                {
                    Data->WriteString(d->Name);
                    break;
                }
                case BT_ACTOR_ID      :
                {
                    Data->WriteByte(d->ID);
                    break;
                }
                case BT_ACTOR_PATH_ID :
                {
                    Data->WriteByte(d->PathID);
                    break;
                }
                case BT_ACTOR_VARIABLES:
                {
                    //To do
                    if(Data->GetSize() > 0)
                    {
                        
                    }
                    break;
                }
                case BT_ACTOR_LINK:
                {
                    break;
                }
                default:
                {
                    //printf("Unsupported actor block type (%s).\n",BlockTypeIDs[cBlock->GetType()].c_str());
                }
            }
        }
    }
};