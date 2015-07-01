#include <ObjLoader.h>

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <assert.h>
#include <map>

using namespace std;

namespace TestClient
{
    void ObjLoader::Load(CString Filename)
    {
        ifstream inf;
        inf.open(Filename, ios_base::in);
        if(!inf.is_open()) ERROR("Failed to open file <%s> for reading.\n",Filename);

        m_Positions.clear();
        m_Normals  .clear();
        m_TexCoords.clear();

        char *delims = " \n\r";
        const unsigned int CHARACTER_COUNT = 500;
        char line[CHARACTER_COUNT] = { 0 };

        std::vector<Vec3> verts;
        std::vector<Vec3> norms;
        std::vector<Vec2> texcoords;
        
        std::vector<u32> vindices;
        std::vector<u32> nindices;
        std::vector<u32> tindices;
        
        while (inf.good())
        {
            memset( (void*)line, 0, CHARACTER_COUNT);
            inf.getline(line, CHARACTER_COUNT);
            if (inf.eof()) break;

            char *token = strtok(line, delims);
            if (token == NULL || token[0] == '#' || token[0] == '$')
                continue;

            // verts look like:
            // v float float float
            if (strcmp(token,"v") == 0)
            {
                float x=0, y=0, z=0;
                sscanf(line+2,"%f %f %f", &x, &y, &z);
                verts.push_back( Vec3(x,y,z) );
            }
            
            // normals:
            // nv float float float
            else if (strcmp(token,"vn") == 0)
            {
                float x=0, y=0, z=0;
                sscanf(line+3,"%f %f %f", &x, &y, &z);
                norms.push_back(Vec3(x,y,z));
            }
            
            // texcoords:
            // vt float float
            else if (strcmp(token,"vt") == 0)
            {
                float x=0, y=0;
                sscanf(line+3,"%f %f", &x, &y);
                texcoords.push_back(Vec2(x,y));
            }

            // keep track of smoothing groups
            // s [number|off]
            else if (strcmp(token,"s") == 0)
            {

            }

            // faces start with:
            // f
            else if (strcmp(token, "f") == 0)
            {
                // fill out a triangle from the line, it could have 3 or 4 edges
                char *lineptr = line + 2;
                while (lineptr[0] != 0) {
                    while (lineptr[0] == ' ') ++lineptr;

                    int vi=0, ni=0, ti=0;
                    if (sscanf(lineptr, "%d/%d/%d", &vi, &ti, &ni) == 3) {
                        vindices.push_back(vi-1);
                        nindices.push_back(ni-1);
                        tindices.push_back(ti-1);
                    }
                    else
                    if (sscanf(lineptr, "%d//%d", &vi, &ni) == 2) {
                        vindices.push_back(vi-1);
                        nindices.push_back(ni-1);
                    }
                    else
                    if (sscanf(lineptr, "%d/%d", &vi, &ti) == 2) {
                        vindices.push_back(vi-1);
                        tindices.push_back(ti-1);
                    }
                    else
                    if (sscanf(lineptr, "%d", &vi) == 1) {
                        vindices.push_back(vi-1);
                    }

                    while(lineptr[0] != ' ' && lineptr[0] != 0) ++lineptr;
                }
            }
        }
        
        for(i32 i = 0;i < vindices.size();i++)
        {
            m_Positions.push_back(verts[vindices[i]]);
        }
        for(i32 i = 0;i < nindices.size();i++)
        {
            m_Normals.push_back  (norms[nindices[i]]);
        }
        for(i32 i = 0;i < tindices.size();i++)
        {
            m_TexCoords.push_back(texcoords[tindices[i]]);
        }
    }
};