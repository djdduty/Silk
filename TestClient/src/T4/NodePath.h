/*
 *  NodePath.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#pragma once
#include <string>
#include <iostream>
using namespace std;

class NodePath
{
    public:
        NodePath() { }
        ~NodePath() { }
    
        bool Load(size_t Size,char* Data);
    
    protected:
        bool LoadBlock(const string& BlockID,size_t Size,char* Data);
};