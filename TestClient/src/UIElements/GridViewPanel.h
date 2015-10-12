#pragma once
#include <Silk.h>
using namespace Silk;

namespace TestClient
{
    class InputManager;
    
    class GridTile : public UIPanel
    {
        public:
            GridTile() : UIPanel(Vec2(0,0)) { }
            virtual ~GridTile() { }
        
            virtual void OnCursorMove     () { }
            virtual void OnMouseButtonDown() { }
            virtual void OnMouseButtonUp  () { }
        
            virtual void OnResize(Scalar sz) = 0;
            virtual void Update  (Scalar dt) = 0;
    };
    
    class GridView : public UIPanel
    {
        public:
            GridView(const Vec2& Size,InputManager* InputMgr);
            ~GridView();
        
            void Clear();
            void AddTile(GridTile* T) { AddChild(T); m_Tiles.push_back(T); UpdatePanels(); }
        
            void OnMouseDown();
        
            void SetTileSpacing(Scalar Sz) { m_TileSpacing = Sz; UpdatePanels(); }
            void SetTileSize   (Scalar Sz) { m_TileSize    = Sz; UpdatePanels(); }
            void SetBorderSize (Scalar Sz) { m_BorderSize  = Sz; UpdatePanels(); }
            void SetTextSize   (Scalar Sz) { m_TextSize    = Sz; UpdatePanels(); }
        
        protected:
            void UpdatePanels();
        
            Scalar m_BorderSize ;
            Scalar m_TextSize   ;
            Scalar m_TileSize   ;
            Scalar m_TileSpacing;
        
            vector<GridTile*> m_Tiles;
        
            InputManager* m_Input;
    };
};