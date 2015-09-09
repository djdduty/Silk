/*
 *  ActorMesh.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/28/15.
 *
 */

#pragma once
#include <stdint.h>
#include <string>
#include <vector>
using namespace std;

#include "MeshStructs.h"

class MTFBlock
{
    public:
        enum BLOCK_TYPE
        {
            BT_FILE,        //ELIF
            BT_MESH,        //HSEM
            BT_TIME,        //EMIT
            BT_TYPE,        //EPYT
            BT_VERSION,     //SREV
            BT_DATE,        //ETAD
            BT_TEXTURE,     //RTXT
            BT_MATERIAL,    //LRTM
            BT_INFO,        //OFNI
            BT_VERTICES,    //STRV
            BT_INDICES,     //XDNI
            BT_NODE,        //EDON
            BT_BOUNDINGBOX, //XOBB
            BT_SIZE,        //EZIS
            BT_BONE,        //ENOB
            BT_FACE,        //ECAF
            BT_SKELETON,    //LEKS
            BT_SUBMESHNAMES,//MNMS
            
            /* Unknown */
            BT_GEOM, //Geometry?
            BT_STAT,
            BT_TXST,
            BT_RNST,
            BT_VPSH,
            BT_VSID,
            BT_COLL, //Collision?
            BT_TRIS, //Triangles?
            BT_SUBS,
            BT_AGEO,
            BT_VER_, //Version?
            BT_VECT, //Vector?
            BT_BOUN, //Bounds?
            BT_CENT, //Center?
            BT_SBOU,
            BT_OCNO,
            BT_FALI,
            BT_CHNK, //Chunk?
            BT_CNKS,
            BT_TGET,
            BT_RDH_,
            BT_SUBC,
            
            BT_COUNT,
        };
    
        MTFBlock() : m_Data(0) { }
        ~MTFBlock() { if(m_Data) delete [] m_Data; m_Data = 0; }
    
        bool Load(FILE* fp);
        BLOCK_TYPE GetType() const { return m_Type; }
        string GetTypeString() const;
    
        int m_Unk0;
        int m_Unk1;
        int m_Unk2;
        int m_DataOffset;
        int m_DataSize;
    
        char* m_Data;
        
    protected:
        BLOCK_TYPE m_Type;
};

class SubMesh;
class MeshChunk
{
    public:
        MeshChunk(SubMesh* m) : m_Mesh(m) { }
        ~MeshChunk() { }
        
        void AddIndex (int16_t i )   { m_Indices.push_back(i); }
        void Remove   (size_t Idx)   { m_Indices.erase(m_Indices.begin() + Idx); }
        size_t GetIndexCount() const { return m_Indices.size(); }
        int16_t GetIndex(size_t Idx) { return m_Indices[Idx]; }
    
    
    protected:
        SubMesh* m_Mesh;
        vector<int16_t> m_Indices;
};

class SubMesh
{
    public:
        SubMesh() { }
        ~SubMesh() { }
    
        void AddVertex(const MTFVertexTypeA& v) { m_VerticesA.push_back(v); }
        void AddVertex(const MTFVertexTypeB& v) { m_VerticesB.push_back(v); }
        void AddVertex(const ATFVertexTypeA& v) { m_VerticesC.push_back(v); }
        void AddIndex (int16_t i) { m_Indices .push_back(i); }
        void AddChunk (MeshChunk* Chunk) { m_Chunks.push_back(Chunk); }
    
        bool GetVertexType() const { return m_vType; }
        size_t GetVertexCount() const;
        void GetVertex  (size_t Idx,float* Ptr) const;
        void GetNormal  (size_t Idx,float* Ptr) const;
        void GetTexCoord(size_t Idx,float* Ptr) const;
        void GetUnk0    (size_t Idx,char * Ptr) const;
        void GetUnk1    (size_t Idx,float* Ptr) const; //MTF-B, ATF-A only
        void GetUnk2    (size_t Idx,char * Ptr) const; //ATF-A only
    
        size_t GetChunkCount() const { return m_Chunks.size(); }
        MeshChunk* GetChunk(size_t Idx) { return m_Chunks[Idx]; }
    
        size_t GetIndexCount() const { return m_Indices.size(); }
        int16_t GetIndex(size_t Idx) { return m_Indices[Idx]; }
    
    protected:
        friend class ActorMesh;
        int m_vType;
        vector<MTFVertexTypeA> m_VerticesA;
        vector<MTFVertexTypeB> m_VerticesB;
        vector<ATFVertexTypeA> m_VerticesC;
        vector<int16_t       > m_Indices  ;
        vector<MeshChunk*    > m_Chunks   ;
};

class ActorMesh
{
    public:
        ActorMesh();
        ~ActorMesh();
    
        bool Load(const string& TurokDir,const string& File);
    
        size_t GetBlockCount() const { return m_Blocks.size(); }
        MTFBlock* GetBlock(size_t Idx) const { return m_Blocks[Idx]; }
    
        size_t GetSubMeshCount() const { return m_SubMeshes.size(); }
        SubMesh* GetSubMesh(size_t Idx) const { return m_SubMeshes[Idx]; }
        
    //protected:
        void ProcessBlock(const string& TurokDir,int Idx);
        bool m_IsAnimated;
        vector<MTFBlock*> m_Blocks;
    
        vector<string  > m_SubMeshNames;
        vector<SubMesh*> m_SubMeshes;
        vector<string  > m_Textures;
        vector<MeshInfo> m_MeshInfos;
    
        vector<MTRL    > m_MTRLs;
        vector<TSNR    > m_TSNRs;
        vector<TXST    > m_TXSTs;
};