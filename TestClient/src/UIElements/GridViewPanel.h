#pragma once
#include <Silk.h>
using namespace Silk;

#include <UIElements/ScrollBar.h>

namespace TestClient
{
    class InputManager;
    
    class GridTile : public UIPanel
    {
        public:
            GridTile() : UIPanel(Vec2(0,0)) { }
            virtual ~GridTile() { }
        
            virtual void GenerateContent(const Vec2& Size) = 0;
    };
    
    class GridView : public UIPanel, ScrollBarEventReceiver
    {
        public:
            GridView(const Vec2& Size,InputManager* InputMgr,UIElement* Parent);
            ~GridView();
        
            void Clear();
			void AddTile(GridTile* T) { AddChild(T); m_Tiles.push_back(T); T->SetEnabled(false); UpdatePanels(); }
            GridTile* GetTile(i32 Idx) const { return m_Tiles[Idx]; }
			void GetVisibleTiles(vector<GridTile*>& Tiles) const;
        
            void OnMouseDown();
            
            virtual void OnScrollChanged(Scalar CurrentValue,Scalar Delta,ScrollBar* Sender);
        
            void SetBorderSize   (Scalar Sz) { m_BorderSize    =  Sz; UpdatePanels(); }
            void SetTileSpacing  (Scalar Sz) { m_TileSpacing   =  Sz; UpdatePanels(); }
            void SetTilesPerRow  (i32 TPR  ) { m_TilesPerRow   = TPR; UpdatePanels(); }
            void SetScrollBarSize(Scalar Sz) { m_ScrollBarSize =  Sz; UpdatePanels(); }
        
        protected:
            void UpdatePanels();
        
            Scalar m_BorderSize   ;
            Scalar m_ScrollBarSize;
            Scalar m_TileSpacing  ;
            i32    m_TilesPerRow  ;
            Scalar m_ScrollOffset ;
        
            ScrollBar* m_ScrollBar;
            vector<GridTile*> m_Tiles;
            vector<GridTile*> m_LastVisibleTiles;
        
            InputManager* m_Input;
    };
};