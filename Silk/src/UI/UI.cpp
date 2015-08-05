#include <UI/UI.h>
#include <Renderer/Renderer.h>

namespace Silk
{
    UIManager::UIManager(Renderer* r) : m_NextUID(0), m_Renderer(r), m_Camera(0), m_ViewScale(1.0f,1.0f), m_Resolution(0.0f,0.0f), m_ViewNeedsUpdate(true)
    {
    }
    UIManager::~UIManager()
    {
        if(m_Camera) delete m_Camera;
    }
    
    void UIManager::Initialize()
    {
        m_Resolution = m_Renderer->GetRasterizer()->GetContext()->GetResolution();
        Vec2 hRes = m_Resolution * 0.5f;
        m_Camera = new Camera(-hRes.x,hRes.x,-hRes.y,hRes.y,0.0f,1.0f);
        m_View = m_Renderer->GetRasterizer()->CreateTexture();
        m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
    }
    
    void UIManager::Render()
    {
        //Update texture and projection if resolution changes
        Vec2 cRes = m_Renderer->GetRasterizer()->GetContext()->GetResolution();;
        if(m_Resolution.x != cRes.x || m_Resolution.y != cRes.y)
        {
            m_Resolution = cRes;
            m_Camera->SetOrthographic(m_Resolution);
            m_ViewNeedsUpdate = true;
            m_View->CreateTexture(m_Resolution.x,m_Resolution.y);
            m_View->UpdateTexture();
        }
        
        if(m_ViewNeedsUpdate)
        {
            m_View->EnableRTT();
            
            m_View->DisableRTT();
        }
    }
};
