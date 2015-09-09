/*
 *  ATRLoader.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/27/15.
 *
 */

#pragma once
#include "ActorMesh.h"
#include "ActorInstances.h"

#include <string>
#include <vector>
using namespace std;

class ATRBlock
{
    public:
        enum BLOCK_TYPE
        {
            BT_ACTOR,
            BT_ACTORMESH,
            BT_ACTORINSTANCES,
            BT_PRECACHEFILE,
            BT_ID,
            BT_POSITION,
            BT_ROTATION,
            BT_SCALE,
            BT_COUNT,
        };
    
        ATRBlock();
        ~ATRBlock();
    
        bool Load(const string& TurokDir,FILE* fp);
        bool Load(const string& TurokDir,char* Data,size_t& DataOffset);
    
        BLOCK_TYPE GetType() const { return m_Type; }
        string GetTypeString() const { return m_TypeString; }
        size_t GetSize() const { return m_Size; }
        char* GetData() const { return m_Data; }
    
    protected:
        friend class ATRFile;
        BLOCK_TYPE m_Type;
        string     m_TypeString;
        size_t     m_Size;
        char*      m_Data;
};

class ATRFile
{
    public:
        ATRFile();
        ~ATRFile();
    
        bool Load(const string& TurokDir,const string& Filename);
    
        string GetErrorString() const { return m_ErrorString; }
    
        size_t GetBlockCount() const { return m_Blocks.size(); }
        ATRBlock* GetBlock(size_t Idx) const { return m_Blocks[Idx]; }
    
        size_t GetActorCount() const { return m_Actors.size(); }
        Actor* GetActor(size_t Idx) const { return m_Actors[Idx]; }
    
        /* For levels only */
        ActorInstances* GetActors() const { return m_ActorInstanceFiles[0]; }
        
    protected:
        friend class Actor;
        bool CheckHeader(FILE* fp);
        bool LoadBlock(FILE* fp);
        bool ProcessBlock(const string& TurokDir,int Idx);
        bool LoadActors();
        
        string m_Filename;
        string m_ErrorString;
        bool   m_IsLevel;
    
        vector<ATRBlock *> m_Blocks;
        vector<Actor*    > m_Actors;
    
        /*
         * Only level .ATR files use these
         */
        vector<ActorInstances*> m_ActorInstanceFiles;
};

