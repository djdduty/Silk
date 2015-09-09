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