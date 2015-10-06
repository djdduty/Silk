/*
 *  Directory.cpp
 *  T4_ATR
 *
 *  Created by Michael DeCicco on 8/28/15.
 *
 */

#include "Directory.h"

#ifdef _WIN32
#include <T4/dirent.h>
#else
#include <dirent.h>
#endif

void DirectoryInfo::Populate(const string& Dir)
{
    if(m_Entries) { delete [] m_Entries; m_Entries = 0; }
    m_EntryCount = 0;
    
    DIR *dp = opendir(Dir.c_str());
    if(dp != NULL)
    {
        struct dirent *ep = readdir(dp);
        while(ep)
        {
            m_EntryCount++;
            ep = readdir(dp);
        }
        rewinddir(dp);
        m_Entries = new DirEnt[m_EntryCount];
        int i = 0;
        ep = readdir(dp);
        while(ep)
        {
            string n = string(ep->d_name,ep->d_namlen);
            if(n.size() != 0 && n != ".DS_Store")
            {
                m_Entries[i].Name = n;
                switch(ep->d_type)
                {
                    case DT_REG: { m_Entries[i].Type = DET_FILE;    break; }
                    case DT_DIR: { m_Entries[i].Type = DET_FOLDER;  break; }
                    case DT_LNK: { m_Entries[i].Type = DET_LINK;    break; }
                    default:     { m_Entries[i].Type = DET_INVALID; break; }
                }
                if(ep->d_type == DT_REG)
                {
                    size_t extbegin = m_Entries[i].Name.rfind('.');
                    if(extbegin != m_Entries[i].Name.npos) m_Entries[i].Extension = m_Entries[i].Name.substr(extbegin + 1);
                    else m_Entries[i].Extension = "";
                }
                i++;
            }
            ep = readdir(dp);
        }
        closedir(dp);
    }
}