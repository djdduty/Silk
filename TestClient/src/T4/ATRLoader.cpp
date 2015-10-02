/*
 *  ATRLoader.cpp
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/27/15.
 *
 */

#include "ATRLoader.h"

#include <iostream>
using namespace std;

//#define printf(...) 
//#define cout clog

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

ATRBlock::ATRBlock() : m_Data(0), m_Size(0)
{
}

ATRBlock::~ATRBlock()
{
    if(m_Data) delete [] m_Data;
}

bool ATRBlock::Load(const string &TurokDir,FILE* fp)
{
    //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
    unsigned char PreBlockFlag = (unsigned char)getc(fp);
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
    string BlockID;
    
    if(!HasUnkBlockHeaderType) //"Normal" block header
    {
        //Hdr[0] = Unknown
        //Hdr[1] = Unknown (when not part of 2-byte block size)
        //Hdr[2] = Block size in bytes (starts after block ID, including the null)
        //Hdr[3] = Block ID string length
        
        //Read block header
        char Hdr[4];
        if(fread(Hdr,4,1,fp) != 1)
        {
            cout << "Failed to read ATR block header (Type A)." << endl;
            fclose(fp);
            return false;
        }
        
        
        //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
        for(int i = 0;i < (unsigned char)Hdr[3];i++) BlockID += getc(fp);
        
        //Null byte (always exists after strings)
        getc(fp);
        
        //Establish block size
        if(!Has16BitBlockSize)
        {
            m_Size = (size_t)(unsigned char)Hdr[2];
            //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
        }
        else
        {
            m_Size = (size_t)*((uint16_t*)&Hdr[1]);
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
                char Hdr[5];
                if(fread(Hdr,5,1,fp) != 1)
                {
                    cout << "Failed to read ATR block header (Type B)." << endl;
                    fclose(fp);
                    return false;
                }
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[4];i++) BlockID += getc(fp);
                
                //Null byte (always exists after strings)
                getc(fp);
                
                //Establish block size
                m_Size = (size_t)(unsigned char)Hdr[1];
                
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
                char Hdr[6];
                if(fread(Hdr,6,1,fp) != 1)
                {
                    cout << "Failed to read ATR block header (Type C)." << endl;
                    fclose(fp);
                    return false;
                }
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += getc(fp);
                
                //Null byte (always exists after strings)
                getc(fp);
                
                //Establish block size
                m_Size = (size_t)*((uint16_t*)&Hdr[1]);
                
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
                char Hdr[6];
                if(fread(Hdr,6,1,fp) != 1)
                {
                    cout << "Failed to read ATR block header (Type D)." << endl;
                    fclose(fp);
                    return false;
                }
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += getc(fp);
                
                //Null byte (always exists after strings)
                getc(fp);
                
                //Establish block size
                m_Size = (size_t)*((uint32_t*)&Hdr[1]);
                
                //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                
                break;
            }
        }
    }
    m_TypeString = BlockID;
    if(m_Size != -1)
    {
        m_Data = new char[m_Size];
        if(fread(m_Data,m_Size,1,fp) != 1)
        {
            cout << "Failed to read ATR block data." << endl;
            fclose(fp);
            delete m_Data;
            return false;
        }
    }
    
    for(int i = 0;i < BT_COUNT;i++)
    {
        if(BlockID == BlockTypeIDStrings[i])
        {
            m_Type = (BLOCK_TYPE)i;
            break;
        }
        else m_Type = BT_COUNT;
    }
    
    return true;
}
bool ATRBlock::Load(const string &TurokDir,char *Data,size_t& DataOffset)
{
    //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
    size_t dPos = 0;
    
    unsigned char PreBlockFlag = (unsigned char)Data[dPos++];
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
        default  : { dPos--; }
    }
    
    //Basic block information
    string BlockID;
    
    if(!HasUnkBlockHeaderType) //"Normal" block header
    {
        //Hdr[0] = Unknown
        //Hdr[1] = Unknown (when not part of 2-byte block size)
        //Hdr[2] = Block size in bytes (starts after block ID, including the null)
        //Hdr[3] = Block ID string length
        
        //Read block header
        char Hdr[4] = { Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++] };
        
        
        //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
        for(int i = 0;i < (unsigned char)Hdr[3];i++) BlockID += Data[dPos++];
        
        //Null byte (always exists after strings)
        dPos++;
        
        //Establish block size
        if(!Has16BitBlockSize)
        {
            m_Size = (size_t)(unsigned char)Hdr[2];
            //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
        }
        else
        {
            m_Size = (size_t)*((uint16_t*)&Hdr[1]);
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
                char Hdr[5] = { Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++] };
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[4];i++) BlockID += Data[dPos++];
                
                //Null byte (always exists after strings)
                dPos++;
                
                //Establish block size
                m_Size = (size_t)(unsigned char)Hdr[1];
                
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
                char Hdr[6] = { Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++] };
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += Data[dPos++];
                
                //Null byte (always exists after strings)
                dPos++;
                
                //Establish block size
                m_Size = (size_t)*((uint16_t*)&Hdr[1]);
                
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
                char Hdr[6] = { Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++], Data[dPos++] };
                
                //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
                for(int i = 0;i < (unsigned char)Hdr[5];i++) BlockID += Data[dPos++];
                
                //Null byte (always exists after strings)
                dPos++;
                
                //Establish block size
                m_Size = (size_t)*((uint32_t*)&Hdr[1]);
                
                //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                
                break;
            }
        }
    }
    
    if(m_Size != -1)
    {
        m_Data = new char[m_Size];
        memcpy(m_Data,&Data[dPos],m_Size);
        DataOffset = dPos + m_Size;
    }
    
    for(int i = 0;i < BT_COUNT;i++)
    {
        if(BlockID == BlockTypeIDStrings[i])
        {
            m_Type = (BLOCK_TYPE)i;
            break;
        }
        else m_Type = BT_COUNT;
    }
    
    return true;
}

ATRFile::ATRFile() : m_IsLevel(false)
{
}

ATRFile::~ATRFile()
{
}

bool ATRFile::Load(const string& TurokDir,const string& Filename)
{
    //Load file
    m_Filename = Filename;
    FILE* fp = fopen(Filename.c_str(),"rb");
    if(!fp) { m_ErrorString = "File not found"; return false; }
    
    if(!CheckHeader(fp)) return false;
    
    //Load contents
    while(getc(fp) != EOF)
    {
        fseek(fp,-1,SEEK_CUR);
        ATRBlock* blk = new ATRBlock();
        if(!blk->Load(TurokDir,fp))
        {
            m_ErrorString = "Failed to load ATR block";
            fclose(fp);
            return false;
        }
        m_Blocks.push_back(blk);
    }
    
    //Close file
    fclose(fp);
    
    for(int i = 0;i < m_Blocks.size();i++)
    {
        ProcessBlock(TurokDir,i);
    }
    
    return true;
}

bool ATRFile::CheckHeader(FILE* fp)
{
    char hdr[4];
    if(fread(hdr,4,1,fp) != 1)
    {
        perror(0);
        m_ErrorString = "Unable to read header from file. (Empty file?)";
        return false;
    }
    
    if(hdr[1] != 'a'
    || hdr[2] != 't'
    || hdr[3] != 'r')
    {
        m_ErrorString = "Invalid file header";
        return false;
    }
    return true;
}
bool ATRFile::ProcessBlock(const string& TurokDir,int Idx)
{
    ATRBlock* b = m_Blocks[Idx];
    char* Data = b->GetData();
    int dPos = 0;
    
    switch(b->GetType())
    {
        case ATRBlock::BT_ACTOR:
        {
            size_t PathLen = (unsigned char)Data[dPos++];
            string ATRPath(&Data[dPos],PathLen); dPos += PathLen + 1;
            ATRPath = ATRPath.substr(ATRPath.find_first_of("Y:") + 2,ATRPath.length() - 1);
            ATRPath = TurokDir + ATRPath;
            for(int s = 0;s < ATRPath.length();s++) { if(ATRPath[s] == '\\') ATRPath[s] = '/'; }
            cout << "Path: " << ATRPath << "." << endl;
            
            Actor* a = new Actor(this);
            m_Actors.push_back(a);
            
            //Check to see if this is a level ATR file
            size_t iPos = 0;
            while(iPos != b->GetSize())
            {
                if(Data[iPos] == 'A' && iPos + 14 < b->GetSize())
                {
                    if(string(&Data[iPos],14) == "ACTORINSTANCES")
                    {
                        m_IsLevel = true;
                        break;
                    }
                }
                iPos++;
            }
            
            while(dPos != b->GetSize())
            {
                size_t Offset = 0;
                ATRBlock* blk = new ATRBlock();
                if(!blk->Load(TurokDir,&Data[dPos],Offset))
                {
                    m_ErrorString = "Failed to load ATR block";
                    delete blk;
                    return false;
                }
                dPos += Offset;
                m_Blocks.push_back(blk);
            }
            
            break;
        }
        case ATRBlock::BT_ACTORMESH:
        {
            size_t PathLen = b->GetSize();
            string _Path(&Data[dPos]); dPos += PathLen + 1;
            string Path;
            int s = 0;
            if(_Path[0] == 'Y' && _Path[1] == ':') s = 2;
            else if(_Path[1] == 'Y' && _Path[2] == ':') s = 3;
            else if(_Path[0] == '$') s = 1;
            
            for(;s < _Path.length();s++)
            {
                if(_Path[s] == '\\') Path += '/';
                else Path += _Path[s];
            }
            Path = TurokDir + Path;
            
            cout << "Path: " << Path << "." << endl;
    
            ActorMesh* m = new ActorMesh();
            if(!m->Load(TurokDir,Path))
            {
                m_ErrorString = string("Failed to load actor mesh <") + Path + ">";
                delete m;
                return false;
            }
            m_Actors[m_Actors.size() - 1]->m_Mesh = m;
            break;
        }
        case ATRBlock::BT_ACTORINSTANCES:
        {
            size_t PathLen = b->GetSize();
            string _Path(&Data[dPos]); dPos += PathLen + 1;
            string Path;
            int s = 0;
            if(_Path[0] == 'Y' && _Path[1] == ':') s = 2;
            else if(_Path[1] == 'Y' && _Path[2] == ':') s = 3;
            else if(_Path[0] == '$') s = 1;
            
            for(;s < _Path.length();s++)
            {
                if(_Path[s] == '\\') Path += '/';
                else Path += _Path[s];
            }
            Path = TurokDir + Path;
            
            cout << "Path: " << Path << "." << endl;
            
            cout << "Loading actor instances <" << Path << ">." << endl;
            ActorInstances* a = new ActorInstances();
            if(!a->Load(TurokDir,Path))
            {
                cout << "Failed to load actor instances <" + Path + ">";
                delete a;
                return false;
            }
            m_ActorInstanceFiles.push_back(a);
            break;
        }
        case ATRBlock::BT_PRECACHEFILE:
        {
            break;
        }
        default:
        {
            printf("Unknown ATR block type.\n");
            return false;
        }
    }
}