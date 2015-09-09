/*
 *  NavData.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#pragma once
#include <string>
#include <iostream>
using namespace std;

class NavData
{
    public:
        NavData() { }
        ~NavData() { }
    
        bool Load(size_t Size,char* Data);
    
    protected:
        bool LoadBlock(const string& BlockID,size_t Size,char* Data);
        bool LoadNodes(size_t Size,char* Data);
        bool LoadLinks(size_t Size,char* Data);
        bool LoadNodeBlock(const string& BlockID,size_t Size,char* Data);
        bool LoadLinkBlock(const string& BlockID,size_t Size,char* Data);
};