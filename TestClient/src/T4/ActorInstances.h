/*
 *  ActorInstances.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/28/15.
 *
 */

#pragma once
#include <string>
#include <vector>
using namespace std;

#include "Actor.h"
#include "NodePath.h"
#include "NavData.h"

class ActorInstances
{
    public:
        ActorInstances() : m_Nav(0) { }
        ~ActorInstances();
    
        bool Load(const string& TurokDir,const string& File);
        bool CheckHeader(FILE* fp);
    
    public:
        vector<Actor   *> m_Actors;
        vector<NodePath*> m_Paths ;
        NavData* m_Nav;
};