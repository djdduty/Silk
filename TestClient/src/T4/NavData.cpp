/*
 *  NavData.cpp
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#include "NavData.h"
#include <stdint.h>

bool NavData::Load(size_t Size,char* Data)
{
    //printf("BEGIN NAVDATA (S: %zu)\n",Size);
    int dPos = 0;
    
    //Load actor blocks
    while(dPos != Size)
    {
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
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
            default  : { dPos--; }
        }
        
        //Basic block information
        string BlockID;
        size_t BlockSize = 0;
        
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
                BlockSize = (size_t)(unsigned char)Hdr[2];
                //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
            }
            else
            {
                BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
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
                    BlockSize = (size_t)(unsigned char)Hdr[1];
                    
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
                    BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                    
                    break;
                }
            }
        }
        
        
        if(!LoadBlock(BlockID,BlockSize,&Data[dPos]))
        {
            cout << "Failed to load ATI block." << endl;
            return false;
        }
        dPos += BlockSize;
    }
    return true;
}
bool NavData::LoadBlock(const string& BlockID,size_t Size,char* Data)
{
    if(BlockID == "NAVNODES") return LoadNodes(Size,Data);
    else if(BlockID == "NAVLINKS") return LoadLinks(Size,Data);
    return true;
}
bool NavData::LoadNodes(size_t Size,char* Data)
{
    //printf("Loading nodes (S: %zu)\n",Size);
    int dPos = 0;
    
    //Load actor blocks
    while(dPos != Size)
    {
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
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
            default  : { dPos--; }
        }
        
        //Basic block information
        string BlockID;
        size_t BlockSize = 0;
        
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
                BlockSize = (size_t)(unsigned char)Hdr[2];
                //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
            }
            else
            {
                BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
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
                    BlockSize = (size_t)(unsigned char)Hdr[1];
                    
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
                    BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                    
                    break;
                }
            }
        }
        
        
        if(!LoadNodeBlock(BlockID,BlockSize,&Data[dPos]))
        {
            cout << "Failed to load NAVNODE block." << endl;
            return false;
        }
        dPos += BlockSize;
    }
    return true;
}
bool NavData::LoadLinks(size_t Size,char* Data)
{
    //printf("Loading node links (S: %zu)\n",Size);
    int dPos = 0;
    
    //Load actor blocks
    while(dPos != Size)
    {
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
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
            default  : { dPos--; }
        }
        
        //Basic block information
        string BlockID;
        size_t BlockSize = 0;
        
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
                BlockSize = (size_t)(unsigned char)Hdr[2];
                //printf("Block:(?: %d | Sz: %zu): %s\n",(int)(unsigned char)Hdr[0],BlockSize,BlockID.c_str());
            }
            else
            {
                BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
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
                    BlockSize = (size_t)(unsigned char)Hdr[1];
                    
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
                    BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
                    
                    //printf("Block:(?: %d | Sz: %zu | ?: %d, %d): %s\n",(int)(unsigned char)Hdr[0],BlockSize,(int)(unsigned char)Hdr[2],(int)(unsigned char)Hdr[3],BlockID.c_str());
                    
                    break;
                }
            }
        }
        
        
        if(!LoadLinkBlock(BlockID,BlockSize,&Data[dPos]))
        {
            cout << "Failed to load NAVLINK block." << endl;
            return false;
        }
        dPos += BlockSize;
    }
    return true;
}
bool NavData::LoadNodeBlock(const string& BlockID,size_t Size,char* Data)
{
    return true;
}
bool NavData::LoadLinkBlock(const string& BlockID,size_t Size,char* Data)
{
    return true;
}