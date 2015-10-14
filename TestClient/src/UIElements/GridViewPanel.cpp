#include <UIElements/GridViewPanel.h>

namespace TestClient
{
    GridView::GridView(const Vec2& Size,InputManager* InputMgr,UIElement* Parent) : UIPanel(Size), m_Input(InputMgr)
    {
        SetBackgroundColor(Vec4(0,0,0,0.75f));
        m_BorderSize    =  2.0f;
        m_TileSpacing   = 10.0f;
        m_TilesPerRow   =     4;
        m_ScrollBarSize = 11.0f;
        m_ScrollOffset  =  0.0f;
        Parent->AddChild(this);
        
        m_ScrollBar = new ScrollBar(Vec2(m_ScrollBarSize,Size.y - (2.0f * m_BorderSize)),InputMgr,this);
        m_ScrollBar->SetReceiver(this);
    }
    GridView::~GridView()
    {
    }

    void GridView::Clear()
    {
    }

    void GridView::OnMouseDown()
    {
    }
    void GridView::OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender)
    {
        Scalar ContentMinY   = m_Tiles[0]->GetPosition().y;
        Scalar ContentMaxY   = m_Tiles[m_Tiles.size() - 1]->GetPosition().y + m_Tiles[m_Tiles.size() - 1]->GetBounds()->GetDimensions().y;
        Scalar ContentHeight = ContentMaxY - ContentMinY;
        Scalar MaxTrans      = ContentHeight - m_Bounds->GetDimensions().y;
        m_ScrollOffset       = -MaxTrans * CurrentValue;
        UpdatePanels();
    }

    void GridView::UpdatePanels()
    {
        Vec2 Sz = m_Bounds->GetDimensions();
        
        m_ScrollBar->SetPosition(Vec3(Sz.x - m_BorderSize - m_ScrollBarSize,m_BorderSize,0.0f));
        m_ScrollBar->SetSize(Vec2(m_ScrollBarSize,Sz.y - (2.0f * m_BorderSize)));
        
        Sz.x -= m_ScrollBarSize + (3.0f * m_BorderSize);
        
        Scalar TotalEmptySpace = m_TileSpacing * Scalar(m_TilesPerRow - 1);
        Scalar TileSize = (Sz.x - TotalEmptySpace) / Scalar(m_TilesPerRow);
        
        i32 Col = 0;
        i32 Row = 0;
        
        for(i32 i = 0;i < m_Tiles.size();i++)
        {
            Scalar xPos = m_BorderSize + (TileSize * Scalar(Col)) + (m_TileSpacing * Scalar(max(Col,0)));
            Scalar yPos = m_BorderSize + (TileSize * Scalar(Row)) + (m_TileSpacing * Scalar(max(Row,0)));
            
            m_Tiles[i]->GenerateContent(Vec2(TileSize,TileSize));
            m_Tiles[i]->SetSize(Vec2(TileSize,TileSize));
            m_Tiles[i]->SetPosition(Vec3(xPos,yPos + m_ScrollOffset,0.0f));
            
            Col++;
            if(Col == m_TilesPerRow) { Col = 0; Row++; }
        }
    }
};
