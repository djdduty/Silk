/*
 *  Actor.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#pragma once
#include <string>
#include <vector>
using namespace std;
class ActorMesh;
class ATRFile;
class ATRBlock;

struct ActorVec3
{
    float x;
    float y;
    float z;
};

class Actor
{
    public:
        Actor(ATRFile* File = 0) : m_Mesh(0), m_File(File)
        {
            Position.x = Position.y = Position.z =
            Rotation.x = Rotation.y = Rotation.z = 0.0f;
            Scale.x = Scale.y = Scale.z = 1.0f;
        }
        ~Actor();

        ActorMesh* GetMesh() const { return m_Mesh; }
        string GetFilename() const;
    
        bool LoadBlock(const string& TurokDir,ATRBlock* Block);
    
        char ID;
        ActorVec3 Position;
        ActorVec3 Rotation;
        ActorVec3 Scale;
        
    protected:
        friend class ATRFile;
        ActorMesh* m_Mesh;
        ATRFile* m_File;
};