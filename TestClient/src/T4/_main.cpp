/*
 * main.cpp
 * T4_ATR
 *
 * Converts Turok: Evolution ATR files to a set of OBJ files
 * and a human-readable representation of the data contained
 * within the file.
 *
 * /

#ifdef __APPLE__
#include <unistd.h>
#elif _WIN32
#include <direct.h>
#endif

#include <iostream>
using namespace std;

#include "ATRLoader.h"
#include "Directory.h"

bool LoadATR(const string& TurokDir,const string& Filename,const string& OutDir);

int main(int ArgC,char *ArgV[])
{
    //Set working directory
    string Path(ArgV[0]);
    for(size_t i = Path.size() - 1;i > 0;i--)
    {
        if(Path[i] == '/' || Path[i] == '\\') { Path = Path.substr(0,i); break; }
    }
    Path += '/';
    chdir("/");
    
    //Check for arguments
    if(ArgC < 3)
    {
        cout << "Usage: T4_ATR <Turok base folder> <ATR file> (Optional -d <Output folder>)" << endl;
        return -1;
    }
    
    //Load ATR file
    string TurokDir = ArgV[1];
    string Filename = ArgV[2];
    if(Filename != "*")
    {
        string OutDir   = Path + Filename.substr(0,Filename.find_first_of("."));
        if(ArgC > 3 && ArgV[3][0] == '-' && (ArgV[3][1] == 'd' || ArgV[3][1] == 'D')) OutDir = string(ArgV[4]);
        
        LoadATR(TurokDir,Path + Filename,Path + OutDir);
    }
    else if(Filename == "*")
    {
        DirectoryInfo dir;
        dir.Populate(Path);
        
        for(int i = 0;i < dir.GetEntryCount();i++)
        {
            DIR_ENTRY_TYPE t = dir.GetEntryType(i);
            if(t == DET_FILE)
            {
                string Name = dir.GetEntryName(i);
                string Ext  = dir.GetEntryExtension(i);
                
                if(Ext == "atr" || Ext == "ATR")
                {
                    string OutDir = Name.substr(0,Name.find_first_of("."));
                    if(ArgC > 3 && ArgV[3][0] == '-' && (ArgV[3][1] == 'd' || ArgV[3][1] == 'D')) OutDir = string(ArgV[4]);
                    
                    LoadATR(TurokDir,Path + Name,Path + OutDir);
                }
            }
        }
    }
    
    return 0;
}

bool LoadATR(const string& TurokDir,const string& Filename,const string& OutDir)
{
    ATRFile atr;
    if(!atr.Load(TurokDir,Filename))
    {
        cout << "Unable to load ATR file <" << Filename << ">. " << atr.GetErrorString() << "." << endl;
        return false;
    }
    
    //Make output directory
    system((string("mkdir -p ") + OutDir).c_str());
    
    //Produce output
    string Synopsis = string("----FILE ") + Filename + "----\n";
    Synopsis += string("\tBlock Count: ") + to_string(atr.GetBlockCount()) + ".\n";
    Synopsis += string("\tGlobal Unk0: ") + to_string((int)atr.m_Unk0    ) + ".\n";
    
    for(int i = 0;i < atr.GetBlockCount();i++)
    {
        Synopsis += string("\n\t----BLOCK ") + to_string(i) + "----\n";
        
        ATRBlock* b = atr.GetBlock(i);
        
        Synopsis += string("\tUnk0: ") + to_string((int)b->m_Unk0) + ".\n";
        Synopsis += string("\tUnk1: ") + to_string((int)b->m_Unk1) + ".\n";
        Synopsis += string("\tUnk2: ") + to_string((int)b->m_Unk2) + ".\n";
        
        string TypeName;
        switch(b->GetType())
        {
            case ATRBlock::BT_ACTOR         : { TypeName = "Actor file"         ; break; }
            case ATRBlock::BT_ACTORINSTANCES: { TypeName = "Actor instance file"; break; }
            case ATRBlock::BT_ACTORMESH     : { TypeName = "Actor mesh file"    ; break; }
            case ATRBlock::BT_PRECACHEFILE  : { TypeName = "Precache file"      ; break; }
        }
        Synopsis += string("\tType: ") + TypeName + ".\n";
        Synopsis += string("\tPath: ") + b->GetPath() + ".\n";
        
        switch(b->GetType())
        {
            case ATRBlock::BT_ACTOR:
            {
                break;
            }
            case ATRBlock::BT_ACTORINSTANCES:
            {
                break;
            }
            case ATRBlock::BT_ACTORMESH:
            {
                ActorMesh* m = b->GetMesh();
                string DataOutputDir = OutDir + "/" + Filename.substr(Filename.find_last_of("/") + 1,Filename.length()) + "/" + "Mesh[" + to_string(i) + "]";
                
                system((string("mkdir -p ") + DataOutputDir).c_str());
                Synopsis += "\t\t----ACTOR MESH----\n";
                for(int j = 0;j < m->GetBlockCount();j++)
                {
                    Synopsis += string("\t\t\t----BLOCK ") + to_string(j) + "----\n";
                    Synopsis += string("\t\t\tType: "    ) + m->GetBlock(j)->GetTypeString()         + ".\n";
                    Synopsis += string("\t\t\tUnk0: "    ) + to_string(m->GetBlock(j)->m_Unk0      ) + ".\n";
                    Synopsis += string("\t\t\tUnk1: "    ) + to_string(m->GetBlock(j)->m_Unk1      ) + ".\n";
                    Synopsis += string("\t\t\tUnk2: "    ) + to_string(m->GetBlock(j)->m_Unk2      ) + ".\n";
                    Synopsis += string("\t\t\tOffset: "  ) + to_string(m->GetBlock(j)->m_DataOffset) + ".\n";
                    Synopsis += string("\t\t\tSize: "    ) + to_string(m->GetBlock(j)->m_DataSize  ) + ".\n";
                    Synopsis += string("\t\t\t----BLOCK ") + to_string(j) + "----\n";
                    
                    if(m->GetBlock(j)->m_DataSize != -1)
                    {
                        string BlockDataFilename = DataOutputDir + "/" + "Block[" + to_string(j) + "](" + m->GetBlock(j)->GetTypeString() + ").dat";
                        FILE* BlockDataOut = fopen(BlockDataFilename.c_str(),"wb");
                        if(!BlockDataOut) continue;
                        {
                            if(fwrite(m->GetBlock(j)->m_Data,m->GetBlock(j)->m_DataSize,1,BlockDataOut) != 1)
                            {
                                cout << "Failed to write " << m->GetBlock(j)->m_DataSize << " bytes to data file." << endl;
                            }
                        }
                        fclose(BlockDataOut);
                    }
                }
                break;
            }
            case ATRBlock::BT_PRECACHEFILE:
            {
                break;
            }
        }
    }
    Synopsis += string("----FILE ") + Filename + "----\n\n";
    
    cout << Synopsis;
    
    string OutFile = (OutDir + Filename.substr(Filename.find_last_of("/"),Filename.find_last_of(".")) + "-Synopsis.txt");
    FILE* SynOut = fopen(OutFile.c_str(),"w");
    if(!SynOut) return false;
    
    fprintf(SynOut,"%s\n",Synopsis.c_str());
    
    fclose(SynOut);
    return true;
}*/
