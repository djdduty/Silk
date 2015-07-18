#pragma once
#include <Renderer/CullingAlgorithm.h>

namespace Silk
{
    class Scene;
    class NullCullingAlgorithm : public CullingAlgorithm
    {
        public:
            NullCullingAlgorithm(Scene* r);
            ~NullCullingAlgorithm();
        
            virtual CullingResult* PerformCulling();
            virtual const char* GetAlgorithmName() const { return "Null Culling Algorithm"; }
    };
};

