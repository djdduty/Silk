#include <Renderer/CullingAlgorithm.h>
#include <Renderer/RenderObject.h>

namespace Silk
{
    CullingResult::CullingResult()
    {
    }
    CullingResult::~CullingResult()
    {
        if(m_VisibleObjects) delete m_VisibleObjects;
    }
};
