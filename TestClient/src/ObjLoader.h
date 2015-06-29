#pragma once
#include <vector>
#include <Math/Math.h>

using namespace Silk;
namespace TestClient
{
    struct Face
    {
        int a;
        int b;
        int c;
    };

    class ObjLoader
    {
        public:
            ObjLoader();

            void load(char* filename);

            int getIndexCount();
            int getVertCount();

            const unsigned int* getFaces();
            const float* getPositions();
            const float* getNormals();

            int getTexCoordLayers();
            const float* getTexCoords(int multiTexCoordLayer);

        private:

            std::vector<Face> Faces;
            std::vector<Vec3> Positions;
            std::vector<Vec3> Normals;

            // obj's only have 1 layer ever
            std::vector<Vec2> TexCoords;
            unsigned int TexCoordLayers;
    };
}