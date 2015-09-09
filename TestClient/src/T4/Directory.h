/*
 *  Directory.h
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/28/15.
 *
 */

#pragma once
#include <string>
using namespace std;

enum DIR_ENTRY_TYPE
{
    DET_FILE,
    DET_FOLDER,
    DET_LINK,
    DET_INVALID,
};

class DirEnt
{
    public:
        DirEnt() : Type(DET_INVALID) { }
        DirEnt(const DirEnt& o);
        ~DirEnt() { }
        
        DIR_ENTRY_TYPE Type;
        string Name;
        string Extension;
};

class DirectoryInfo
{
    public:
        DirectoryInfo() : m_Entries(0), m_EntryCount(0) { }
        ~DirectoryInfo() { if(m_Entries) delete [] m_Entries; }
    
        void Populate(const string& Dir);
        int GetEntryCount() const { return m_EntryCount; }
        DIR_ENTRY_TYPE GetEntryType(int Idx) const { return m_Entries[Idx].Type; }
        string GetEntryName(int Idx) const { return m_Entries[Idx].Name; }
        string GetEntryExtension(int Idx) const { return m_Entries[Idx].Extension; }
    
        /* std::vector refuses to work in this case... */
        DirEnt* m_Entries;
        int m_EntryCount;
};