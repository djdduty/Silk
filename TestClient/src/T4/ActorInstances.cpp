/*
 *  ActorInstances.cpp
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/28/15.
 *
 */

#include "ActorInstances.h"
#include "ATRLoader.h"
#include <iostream>
using namespace std;

ActorInstances::~ActorInstances()
{
    for(int i = 0;i < m_Actors.size();i++) delete m_Actors[i];
    for(int i = 0;i < m_Paths .size();i++) delete m_Paths [i];
    if(m_Nav) delete m_Nav;
}
bool ActorInstances::Load(const string& TurokDir,const string& File)
{
    FILE* fp = fopen(File.c_str(),"rb");
    if(!fp) return false;
    
    if(!CheckHeader(fp)) { fclose(fp); return false; }
    
    //Load contents
    while(getc(fp) != EOF)
    {
        fseek(fp,-1,SEEK_CUR);
        
        size_t BlockOffset = ftell(fp);
        
        //Determine whether or not the block's size is a 16 bit integer or an 8 bit integer
        unsigned char PreBlockFlag = getc(fp);
        bool Has16BitBlockSize = PreBlockFlag == 0x82 || PreBlockFlag == 0x81;
        if(!Has16BitBlockSize) fseek(fp,-1,SEEK_CUR);
        
        //Hdr[0] = Unknown
        //Hdr[1] = Unknown (when not part of 2-byte block size)
        //Hdr[2] = Block size in bytes (starts after block ID, including the null)
        //Hdr[3] = Block ID string length
        
        //Read block header
        char Hdr[4];
        if(fread(Hdr,4,1,fp) != 1)
        {
            cout << "Failed to read ATI block header." << endl;
            return false;
        }
        
        //Read block ID string (ACTOR, ID, ACTOR_VARIABLES, EVENTS, etc...)
        string BlockID;
        for(int i = 0;i < (unsigned char)Hdr[3];i++) BlockID += getc(fp);
        
        //Establish block size
        size_t BlockSize = 0;
        if(!Has16BitBlockSize) BlockSize = (size_t)(unsigned char)Hdr[2];
        else BlockSize = (size_t)*((uint16_t*)&Hdr[1]);
        
        //Null byte (always exists after strings)
        getc(fp);
        
        char* Data = new char[BlockSize];
        if(fread(Data,BlockSize,1,fp) != 1)
        {
            cout << "Failed to read block data." << endl;
            delete [] Data;
            return false;
        }
        
        //cout << "Loading block from offset: " << BlockOffset << "." << endl;
        
        if(BlockID == "ACTOR")
        {
            //Get actor ATR path
            string ATRPath(&Data[1]);
            ATRPath = ATRPath.substr(ATRPath.find_first_of("Y:") + 2,ATRPath.length() - 1);
            ATRPath = TurokDir + ATRPath;
            for(int s = 0;s < ATRPath.length();s++) { if(ATRPath[s] == '\\') ATRPath[s] = '/'; }
            
            ATRFile* ATR = new ATRFile();
            if(!ATR->Load(TurokDir,ATRPath.c_str()))
            {
                cout << "Failed to load actor instance." << endl;
                fclose(fp);
                delete [] Data;
                delete ATR;
                return false;
            }
            
            int dPos = 0;
            
            //Parse to end of the ATR filename
            while(Data[dPos] != 0) dPos++;
            
            //Null after the path
            dPos++;
            
            //Read properties (Position, Rotation, Scale, etc...)
            while(dPos != BlockSize)
            {
                ATRBlock* blk = new ATRBlock();
                size_t Offset = 0;
                if(!blk->Load(TurokDir,&Data[dPos],Offset))
                {
                    cout << "Failed to load ATR block." << endl;
                    fclose(fp);
                    delete [] Data;
                    delete ATR;
                    return false;
                }
                dPos += Offset;
                
                ATR->GetActor(0)->LoadBlock(TurokDir,blk);
                
                delete blk;
            }
        
            m_Actors.push_back(ATR->GetActor(0));
        }
        else if(BlockID == "PATH")
        {
            NodePath* p = new NodePath();
            if(!p->Load(BlockSize,Data))
            {
                cout << "Failed to load node path." << endl;
                fclose(fp);
                delete [] Data;
                delete p;
                return false;
            }
            m_Paths.push_back(p);
        }
        else if(BlockID == "NAVDATA")
        {
            if(m_Nav)
            {
                cout << "You know what to do now." << endl;
            }
            m_Nav = new NavData();
            if(!m_Nav->Load(BlockSize,Data))
            {
                cout << "Failed to load NAVDATA." << endl;
                fclose(fp);
                delete [] Data;
                delete m_Nav;
                return false;
            }
        }
        else
        {
            return false;
        }
        delete [] Data;
    }
    
    fclose(fp);
    return true;
}
bool ActorInstances::CheckHeader(FILE* fp)
{
    char hdr[4];
    if(fread(hdr,4,1,fp) != 1)
    {
        perror(0);
        cout << "Unable to read header from file. (Empty file?)." << endl;
        return false;
    }
    
    if(hdr[1] != 'a'
    || hdr[2] != 't'
    || hdr[3] != 'i')
    {
        cout << "Incorrect header, not actor instances file." << endl;
        return false;
    }
    return true;
}