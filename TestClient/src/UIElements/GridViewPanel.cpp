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
        m_TileSize      =  0.0f;
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
	void GridView::GetVisibleTiles(vector<GridTile*>& Tiles) const
	{
        /*
		Vec2 Sz = m_Bounds->GetDimensions();
        Sz.x -= m_ScrollBarSize + (3.0f * m_BorderSize);
        Scalar TotalEmptySpace = m_TileSpacing * Scalar(m_TilesPerRow - 1);
        Scalar TileSize = (Sz.x - TotalEmptySpace) / Scalar(m_TilesPerRow);

		Scalar MinY,MaxY;
		MinY = -m_ScrollOffset;
		MaxY = MinY + Sz.y;
		
		i32 StartRow = floor(MinY / (TileSize + m_TileSpacing));
		i32 EndRow   = ceil (MaxY / (TileSize + m_TileSpacing));

		i32 StartIdx = StartRow * m_TilesPerRow;
		i32 EndIdx   = EndRow   * m_TilesPerRow;

		for(i32 i = 0;i < EndIdx;i++)
		{
			Tiles.push_back(m_Tiles[StartIdx + i]);
		}
		*/
		Tiles = m_LastVisibleTiles;
	}

    void GridView::OnMouseDown()
    {
    }
    void GridView::OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender)
    {
        Vec2 Sz = m_Bounds->GetDimensions();
        Sz.x -= m_ScrollBarSize + (3.0f * m_BorderSize);
        Scalar TotalEmptySpace = m_TileSpacing * Scalar(m_TilesPerRow - 1);
        Scalar TileSize = (Sz.x - TotalEmptySpace) / Scalar(m_TilesPerRow);
		Scalar ContentHeight = (ceil(Scalar(m_Tiles.size() / m_TilesPerRow)) * (TileSize + m_TileSpacing)) - m_TileSpacing;
		ContentHeight += (m_BorderSize * 2.0f) - 1.0f;

        Scalar MaxTrans      = ContentHeight - m_Bounds->GetDimensions().y;
        m_ScrollOffset       = MaxTrans * CurrentValue;
		if(ContentHeight <= Sz.y) m_ScrollOffset = 0;
        UpdatePanels();
    }

    void GridView::UpdatePanels()
    {
        Vec2 Sz = m_Bounds->GetDimensions();
        
        m_ScrollBar->SetPosition(Vec3(Sz.x - m_BorderSize - m_ScrollBarSize,m_BorderSize,0.0f));
        m_ScrollBar->SetSize(Vec2(m_ScrollBarSize,Sz.y - (2.0f * m_BorderSize)));
        if(m_Tiles.size() == 0) return;

        Sz.x -= m_ScrollBarSize + (3.0f * m_BorderSize);
        
        Scalar TotalEmptySpace = m_TileSpacing * Scalar(m_TilesPerRow - 1);
        Scalar TileSize = (Sz.x - TotalEmptySpace) / Scalar(m_TilesPerRow);
        m_TileSize = TileSize;
        
		Scalar MinY,MaxY;
		MinY = m_ScrollOffset;
		MaxY = MinY + Sz.y;
		
		i32 StartRow = floor(MinY / (TileSize + m_TileSpacing));
		i32 EndRow   = ceil (MaxY / (TileSize + m_TileSpacing));

		i32 StartIdx = StartRow * m_TilesPerRow;
		i32 EndIdx   = EndRow   * m_TilesPerRow;

        i32 Col = 0;
        i32 Row = StartRow;

		for(i32 i = 0;i < m_LastVisibleTiles.size();i++) m_LastVisibleTiles[i]->SetEnabled(false);
		m_LastVisibleTiles.clear();

		for(i32 i = 0;i < (EndIdx - StartIdx);i++)
        {
			if(StartIdx + i >= m_Tiles.size()) break;

            Scalar xPos = m_BorderSize + (TileSize * Scalar(Col)) + (m_TileSpacing * Scalar(max(Col,0)));
            Scalar yPos = m_BorderSize + (TileSize * Scalar(Row)) + (m_TileSpacing * Scalar(max(Row,0))) - 1.0f;
            
            m_Tiles[StartIdx + i]->GenerateContent(Vec2(TileSize,TileSize));
            m_Tiles[StartIdx + i]->SetSize        (Vec2(TileSize,TileSize));
            m_Tiles[StartIdx + i]->SetPosition(Vec3(xPos,yPos - m_ScrollOffset,0.0f));
            
            Col++;
            if(Col == m_TilesPerRow) { Col = 0; Row++; }
			m_LastVisibleTiles.push_back(m_Tiles[StartIdx + i]);
			m_Tiles[StartIdx + i]->SetEnabled(true);
        }
    }
};
