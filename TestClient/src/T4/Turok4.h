/*
 *  Turok4.h
 *  T4_Viewer
 *
 *  Created by Michael DeCicco on 10/1/15.
 *
 */

#pragma once
#include <string>
#include <vector>
#include <stdint.h>
using namespace std;

#include "MeshStructs.h"
namespace Turok4
{
    void SetTurokDirectory(const string& TurokDir);
    string GetTurokDirectory();
    string TransformPseudoPathToRealPath(const string& PseudoPath);
    string TransformRealPathToPseudoPath(const string& RealPath  );

    class SubMesh;
    class ATRFile;
    class ATIFile;
    class Block;
    
    class ByteStream
    {
        public:
            ByteStream(FILE* fp) : m_Offset(0) { while(!feof(fp)) { m_Bytes.push_back(getc(fp)); } }
            ByteStream() : m_Offset(0) { }
            ~ByteStream() { }
        
            bool WriteByte  (char  b);
            bool WriteInt32 (int   i);
            bool WriteInt16 (int   i);
            bool WriteFloat (float f);
            bool WriteString(const string& s);
            bool WriteData  (size_t Sz,void* Ptr);
        
            char    GetByte ();
            int     GetInt32();
            int16_t GetInt16();
            float   GetFloat();
            string  GetString(size_t Len = 0);
            bool    GetData(size_t Sz,void* Ptr);
            ByteStream* SubData(size_t Sz);
        
            int GetOffset() const { return m_Offset; }
            void SetOffset(int Offset) { m_Offset = Offset; }
            void Offset(int Offset) { m_Offset += Offset; }
            size_t GetSize() const { return m_Bytes.size(); }
            void* Ptr() { return &m_Bytes[m_Offset]; }
        
            bool AtEnd(int EndOffset = 0) const { return m_Offset + EndOffset >= m_Bytes.size(); }
        
        protected:
            int m_Offset;
            vector<char> m_Bytes;
    };
    
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
        
            bool Load(const string& File);
        
            size_t GetBlockCount() const { return m_Blocks.size(); }
            MTFBlock* GetBlock(size_t Idx) const { return m_Blocks[Idx]; }
        
            size_t GetSubMeshCount() const { return m_SubMeshes.size(); }
            SubMesh* GetSubMesh(size_t Idx) const { return m_SubMeshes[Idx]; }
            
        //protected:
            void ProcessBlock(int Idx);
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

    struct ActorVec3
    {
        float x;
        float y;
        float z;
    };
    
    struct ActorDef
    {
        string     ActorFile;
        ATRFile*   Actor    ;
        int        BlockIdx ;
        ATIFile*   Parent   ;
        
        //Actor properties
        int        ID       ;
        int        PathID   ;
        ActorMesh* Mesh     ;
        ActorVec3  Position ;
        ActorVec3  Rotation ;
        ActorVec3  Scale    ;
        string     Name     ;
    };
    
    class ActorVariables
    {
        public:
            ActorVariables();
            ~ActorVariables();
        
            bool Load(ByteStream* Data);
            bool Save(ByteStream* Data);
        
            int GetBlockCount() const { return m_Blocks.size(); }
            const Block* GetBlock(int Idx) const { return m_Blocks[Idx]; }
        
            ActorVec3 Spin;
        
        protected:
            void ProcessBlocks();
            vector<Block*> m_Blocks;
    };

    class Actor
    {
        public:
            Actor(ATRFile* File = 0) : m_Mesh(0), m_Def(0), m_Variables(0), m_File(File) { }
            ~Actor();

            ActorMesh* GetMesh() const { return m_Mesh; }
            string GetFilename() const;
        
            ActorDef* GetDef() const { return m_Def; }
        
            void SetActorVariables(ActorVariables* v) { m_Variables = v; }
            ActorVariables* GetActorVariables() const { return m_Variables; }
            
        protected:
            friend class ATRFile;
            friend class ATIFile;
        
            ActorMesh* m_Mesh;
            ActorDef* m_Def;
            ActorVariables* m_Variables;
            ATRFile* m_File;
    };

    class ATRFile
    {
        public:
            ATRFile();
            ~ATRFile();
        
            bool Load(const string& Filename);
            bool Save(const string& Filename);
        
            /* For singular actors only */
            Actor* GetActor() const { return m_Actor; }
        
            /* For levels only */
            ATIFile* GetActors() const { return m_ActorInstanceFiles[0]; }
        
            /* All actors */
            float     GetVersion      () const { return m_Version      ; }
            string    GetActorCode    () const { return m_ActorCode    ; }
            ActorVec3 GetMeshAxis     () const { return m_ActorMeshAxis; }
            string    GetActorMeshFile() const { return m_ActorMeshFile; }
            string    GetInstancesFile() const { return m_InstancesFile; }
            string    GetPrecacheFile () const { return m_PrecacheFile ; }
            
        protected:
            friend class Actor;
            char m_Hdr[4];
            bool CheckHeader();
            void ProcessBlocks();
        
            float     m_Version      ;
            string    m_ActorCode    ;
            ActorVec3 m_ActorMeshAxis;
            string    m_ActorMeshFile;
            string    m_InstancesFile;
            string    m_PrecacheFile ;
        
            ByteStream* m_Data;
            Block* m_Root;
            string m_File;
        
            /*
             * Only actor .ATR files use these
             */
            ATIFile* m_Parent;
            Actor* m_Actor;
        
            /*
             * Only level .ATR files use these
             */
            vector<ATIFile*> m_ActorInstanceFiles;
    };

    
    enum BLOCK_TYPE
    {
        //ATI root block IDs
        BT_ACTOR,
        BT_PATH,
        BT_NAVDATA,
        
        //Actor properties
        BT_ACTOR_ID,
        BT_ACTOR_POSITION,
        BT_ACTOR_ROTATION,
        BT_ACTOR_SCALE,
        BT_ACTOR_NAME,
        BT_ACTOR_VARIABLES,
        BT_ACTOR_LINK,
        BT_ACTOR_CAUSE,
        BT_ACTOR_EVENT,
        BT_ACTOR_PATH_ID,
        BT_ACTOR_CODE,
        BT_ACTOR_MESH_AXIS,
        BT_ACTOR_MESH_BOUNDS,
        BT_ACTOR_PROPERTIES,
        BT_LINK_LISTS,
        BT_ACTOR_TEXTURE_SET,
        BT_GRND_Y,
        BT_MODES,
        
        //Actor data
        BT_ACTOR_MESH,
        BT_ACTOR_INSTANCES,
        BT_ACTOR_PRECACHE_FILE,
        
        //Actor variables
        BT_AFFECTS_TARGET,
        BT_TIME_TO_REACH_MAX_STRENGTH,
        BT_TIME_TO_TURN_OFF,
        BT_HIDE,
        BT_HOLD,
        BT_GA_GROUP_NAME,
        BT_GA_MIN_NUMBERS,
        BT_GA_GROUP_ATTACK,
        BT_SXZ_ANGLE,
        BT_SOUND_RADIUS,
        BT_CLOSE_RANGE_DIST,
        BT_MEDIUM_RANGE_DIST,
        BT_AU_A,
        BT_AU_B,
        BT_AU_C,
        BT_AU_D,
        BT_HEALTH,
        BT_MAX_HEALTH,
        BT_TURN_ON,
        BT_TYPE,
        BT_COUNTERS,
        BT_ONLY_P_TARGET,
        BT_SPAWN_ACTOR1_CHANCE,
        BT_CLOSE_RANGE_CHANCE,
        BT_IGNORE_PLAYER,
        BT_PROVOKE_ONLY,
        BT_FD_MULT,
        BT_COLLIDES,
        BT_IGNORES,
        BT_TOUCHES,
        BT_LIGHT_COLOR,
        BT_LIGHT_INTENSITY,
        BT_FRICTION,
        BT_SPIN_X,
        BT_SPIN_Y,
        BT_SPIN_Z,
        BT_A_NDD,
        BT_A_MDD,
        BT_A_SON,
        BT_A_FL,
        BT_C_B,
        BT_F_B,
        BT_F_C,
        BT_F_F,
        BT_F_T,
        BT_F_ED,
        BT_SM_TYPE,
        BT_LEASH_RADIUS,
        BT_LD_SOUND,
        BT_MD_SOUND,
        BT_HD_SOUND,
        BT_SIGHT_RADIUS,
        BT_SY_ANGLE,
        BT_ATTACK_RESET_TIME,
        BT_USE_HEAD_TRACKING,
        BT_INITIAL_STATE,
        BT_TURNING_SPEED,
        BT_FLAP_BEHAVIOR,
        BT_UPGRADE_1_SLOT_0,
        BT_UPGRADE_2_SLOT_0,
        BT_WEAPON_SLOT_0,
        BT_WEAPON_SLOT_1,
        BT_WEAPON_SLOT_2,
        BT_WEAPON_SLOT_3,
        BT_WEAPON_SLOT_4,
        BT_WEAPON_SLOT_5,
        BT_WEAPON_SLOT_6,
        BT_WEAPON_SLOT_7,
        BT_STARTS_OFF_WITH_WEAPON_0,
        BT_STARTS_OFF_WITH_WEAPON_1,
        BT_STARTS_OFF_WITH_WEAPON_2,
        BT_STARTS_OFF_WITH_WEAPON_3,
        BT_STARTS_OFF_WITH_WEAPON_4,
        BT_STARTS_OFF_WITH_WEAPON_5,
        BT_STARTS_OFF_WITH_WEAPON_6,
        BT_STARTS_OFF_WITH_WEAPON_7,
        BT_ALWAYS_RUN,
        BT_DEF_TRANS_BLEND,
        
        //Unknown
        BT_DUMMIES,
        BT_DEFT,
        BT_COLS,
        BT_VERSION,
        BT_HOTPS,
        BT_TRNS,
        BT_LINK,
        BT_LINKS,
        
        BT_COUNT,
    };
    
    BLOCK_TYPE GetBlockTypeFromID(const string& ID);
    
    class Block
    {
        public:
            Block();
            ~Block();
            
            bool Load(ByteStream* Data);
            bool Save(ByteStream* Data);
        
            void AddChildBlock(Block* b) { m_Children.push_back(b); }
            int GetChildCount() const { return m_Children.size(); }
            Block* GetChild(int Idx) const { return m_Children[Idx]; }
            BLOCK_TYPE GetType() const { return m_Type; }
            string GetTypeString() const { return m_BlockID; }
        
            ByteStream* GetData() const { return m_Data; }
        
        protected:
            unsigned char m_PreBlockFlag;
            char m_Hdr[8];
            string m_BlockID;
            BLOCK_TYPE m_Type;
            ByteStream* m_Data;
            vector<Block*> m_Children;
    };
    
    class ATIFile
    {
        public:
            ATIFile();
            ~ATIFile();
        
            bool Load(const string& File);
            bool Save(const string& File);
        
            int GetActorCount() const { return m_Actors.size(); }
            const ActorDef* GetActorDef(int Idx) const { return m_Actors[Idx]; }
        
            string GetFile() const { return m_File; }
        
        protected:
            void ProcessBlocks();
            void ProcessActorBlock(int Idx);
            void SaveActorBlock(int Idx);
            
            char m_Hdr[4];
            vector<Block*>    m_Blocks;
            vector<ActorDef*> m_Actors;
        
            string m_File;
    };
};

