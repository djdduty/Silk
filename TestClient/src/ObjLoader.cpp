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

        const char *delims = " \n\r";
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
            // v f32 f32 f32
            if (strcmp(token,"v") == 0)
            {
                f32 x=0, y=0, z=0;
                sscanf(line+2,"%f %f %f", &x, &y, &z);
                verts.push_back( Vec3(x,y,z) );
            }
            
            // normals:
            // nv f32 f32 f32
            else if (strcmp(token,"vn") == 0)
            {
                f32 x=0, y=0, z=0;
                sscanf(line+3,"%f %f %f", &x, &y, &z);
                norms.push_back(Vec3(x,y,z));
            }
            
            // texcoords:
            // vt f32 f32
            else if (strcmp(token,"vt") == 0)
            {
                f32 x=0, y=0;
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
    void ObjLoader::ComputeTangents()
    {
        //Stolen from Assimp
        m_Tangents.resize(m_Positions.size());
        for(i32 a = 0;a < m_Positions.size();a += 3)
        {
            i32 p0 = a + 0,
                p1 = a + 1,
                p2 = a + 2;

            //position differences p1->p2 and p1->p3
            Vec3 v = m_Positions[p1] - m_Positions[p0],
                 w = m_Positions[p2] - m_Positions[p0];

            //texture offset p1->p2 and p1->p3
            f32 sx = m_TexCoords[p1].x - m_TexCoords[p0].x, sy = m_TexCoords[p1].y - m_TexCoords[p0].y;
            f32 tx = m_TexCoords[p2].x - m_TexCoords[p0].x, ty = m_TexCoords[p2].y - m_TexCoords[p0].y;
            
            f32 dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

            Vec3 tangent, bitangent;
            tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
            tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
            tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
            
            bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
            bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
            bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

            //store for every vertex of that face
            for(i32 b = 0;b < 3;b++)
            {
                i32 p = a + b;

                //project tangent and bitangent into the plane formed by the vertex' normal
                Vec3 localTangent   = tangent   - m_Normals[p] * (tangent   * m_Normals[p]);
                Vec3 localBitangent = bitangent - m_Normals[p] * (bitangent * m_Normals[p]);
                
                localTangent  .Normalize();
                localBitangent.Normalize();

                m_Tangents[p] = localTangent;
            }
        }
    }
};