/*
 *  Actor.cpp
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#include "Actor.h"
#include "ActorMesh.h"
#include "ATRLoader.h"
#include <iostream>
using namespace std;

//#define printf(...) 
//#define cout clog

Actor::~Actor()
{
    if(m_Mesh) delete m_Mesh;
    //if(m_File) delete m_File;
}
string Actor::GetFilename() const
{
    if(!m_File) return "";
    return m_File->m_Filename;
}

bool Actor::LoadBlock(const string& TurokDir,ATRBlock* b)
{
    int dPos = 0;
    char* Data = b->GetData();
    
    switch(b->GetType())
    {
        case ATRBlock::BT_ID:
        {
            ID = *b->GetData();
            break;
        }
        case ATRBlock::BT_POSITION:
        {
            memcpy(&Position,b->GetData(),sizeof(ActorVec3));
            break;
        }
        case ATRBlock::BT_ROTATION:
        {
            memcpy(&Rotation,b->GetData(),sizeof(ActorVec3));
            break;
        }
        case ATRBlock::BT_SCALE:
        {
            memcpy(&Scale,b->GetData(),sizeof(ActorVec3));
            break;
        }
        default:
        {
            printf("Unknown ATR block type %s.\n",b->GetTypeString().c_str());
            return false;
        }
    }
    return true;
}
bool Actor::SaveTransform()
{
    if(m_ATIFile.length() == 0 || m_ATIOffset.size() == 0) return false;
    FILE* fp = fopen(m_ATIFile.c_str(),"rb+");
    
    bool WrotePos = false;
    bool WroteRot = false;
    bool WroteScl = false;
    
    for(int i = 0;i < m_ATIOffset.size();i++)
    {
        fseek(fp,m_ATIOffset[i],SEEK_SET);
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
        unsigned char PreBlockFlag = getc(fp);
        bool Has16BitBlockSize = false;
        bool HasUnkBlockHeaderType = false;
        int UnkBlockHeaderType = 0;
        
        switch(PreBlockFlag)
        {
            case 0x82: { Has16BitBlockSize     = true; break; }
            case 0x81: { Has16BitBlockSize     = true; break; }
            case 0x61: { HasUnkBlockHeaderType = true; UnkBlockHeaderType = 0; break; }
            case 0xA1: { HasUnkBlockHeaderType = true; UnkBlockHeaderType = 1; break; }
            case 0xC2: { HasUnkBlockHeaderType = true; UnkBlockHeaderType = 2; break; }
            default  : { fseek(fp,-1,SEEK_CUR); }
        }
        
        //Basic block information
        string BlockID;
        size_t Size;
        
        if(!HasUnkBlockHeaderType) //"Normal" block header
        {
            //Hdr[0] = Unknown
            //Hdr[1] = Unknown (when not part of 2-byte block size)
            //Hdr[2] = Block size in bytes (starts after block ID, including the null)
            //Hdr[3] = Block ID string length
            
            //Read block header
            char Hdr[4] = { static_cast<char>(getc(fp)),
                            static_cast<char>(getc(fp)),
                            static_cast<char>(getc(fp)),
                            static_cast<char>(getc(fp)) };
            
            
            //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
            for(int i = 0;i < (unsigned char)Hdr[3];i++) BlockID += getc(fp);
            
            //Null byte (always exists after strings)
            getc(fp);
            
            //Establish block size
            if(!Has16BitBlockSize)
            {
                Size = (size_t)(unsigned char)Hdr[2];
                //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
            }
            else
            {
                Size = (size_t)*((uint16_t*)&Hdr[1]);
                //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
            }
        }
        else if(HasUnkBlockHeaderType)
        {
            switch(UnkBlockHeaderType)
            {
                case 0:
                {
                    //Hdr[0] = Unknown
                    //Hdr[1] = Block size in bytes (starts after block ID, including the null)
                    //Hdr[2] = Unknown
                    //Hdr[3] = Unknown
                    //Hdr[4] = Block ID string length
                    
                    //Read block header
                    char Hdr[5] = { static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)) };
                    
                    //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                    for(int i = 0;i < (unsigned char)Hdr[4];i++) BlockID += getc(fp);
                    
                    //Null byte (always exists after strings)
                    getc(fp);
                    
                    //Establish block size
                    Size = (size_t)(unsigned char)Hdr[1];
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                    break;
                }
                case 1:
                {
                    //Hdr[0] = Unknown
                    //Hdr[1] = First 8 bits of 16 bit integer representing block size
                    //Hdr[2] = Last  8 bits of 16 bit integer representing block size
                    //Hdr[3] = Unknown
                    //Hdr[4] = Unknown
                    //Hdr[5] = Block ID string length
                    
                    //Read block header
                    char Hdr[6] = { static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)) };
                    
                    //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                    for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += getc(fp);
                    
                    //Null byte (always exists after strings)
                    getc(fp);
                    
                    //Establish block size
                    Size = (size_t)*((uint16_t*)&Hdr[1]);
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                
                    break;
                }
                case 2:
                {
                    //Hdr[0] = Unknown
                    //Hdr[1] = First  8 bits of 32 bit integer representing block size
                    //Hdr[2] = Second 8 bits of 32 bit integer representing block size
                    //Hdr[3] = Third  8 bits of 32 bit integer representing block size
                    //Hdr[4] = Last   8 bits of 32 bit integer representing block size
                    //Hdr[5] = Block ID string length
                    
                    //Read block header
                    char Hdr[6] = { static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)),
                                    static_cast<char>(getc(fp)) };
                    
                    //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                    for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += getc(fp);
                    
                    //Null byte (always exists after strings)
                    getc(fp);
                    
                    //Establish block size
                    Size = (size_t)*((uint32_t*)&Hdr[1]);
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                    
                    break;
                }
            }
        }
        
        static string BlockTypeIDStrings[] =
        {
            "ACTOR",
            "ACTOR_MESH",
            "ACTORINSTANCES",
            "PRECACHEFILE",
            "ID",
            "POS",
            "ROT",
            "SCALE",
        };
        
        ATRBlock::BLOCK_TYPE Type;
        for(int i = 0;i < ATRBlock::BT_COUNT;i++)
        {
            if(BlockID == BlockTypeIDStrings[i])
            {
                Type = (ATRBlock::BLOCK_TYPE)i;
                break;
            }
            else Type = ATRBlock::BT_COUNT;
        }
        
        if(Size != -1)
        {
            fseek(fp,0,SEEK_CUR);
            if(Type == ATRBlock::BT_POSITION)
            {
                WrotePos = true;
                if(fwrite(&Position,sizeof(ActorVec3),1,fp) != 1)
                {
                    fclose(fp);
                    cout << "Could not save position to ati file.\n";
                    break;
                }
            }
            else if(Type == ATRBlock::BT_ROTATION)
            {
                WroteRot = true;
                if(fwrite(&Rotation,sizeof(ActorVec3),1,fp) != 1)
                {
                    fclose(fp);
                    cout << "Could not save rotation to ati file.\n";
                    break;
                }
            }
            else if(Type == ATRBlock::BT_SCALE)
            {
                WroteScl = true;
                if(fwrite(&Scale,sizeof(ActorVec3),1,fp) != 1)
                {
                    fclose(fp);
                    cout << "Could not save scale to ati file.\n";
                    break;
                }
            }
            else fseek(fp,Size,SEEK_CUR);
        }
        else
        {
            printf(".\n");
        }
        if(WroteScl && WrotePos && WroteRot) break;
    }
    fclose(fp);
    return true;
}