#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <vector>
using namespace std;

namespace Silk
{
    class UIRect
    {
        public:
            UIRect() : m_DidUpdate(true) { }
            UIRect(const Vec2& Pos,const Vec2& Size)    { Set(Pos,Size); }
            UIRect(Scalar x,Scalar y,Scalar w,Scalar h) { Set(x,y,w,h); }
            ~UIRect() { }
        
            void Set(const Vec2& Pos,const Vec2& Size)    { m_Position = Pos      ; m_Dimensions = Size     ; m_DidUpdate = true; }
            void Set(Scalar x,Scalar y,Scalar w,Scalar h) { m_Position = Vec2(x,y); m_Dimensions = Vec2(w,h); m_DidUpdate = true; }
        
            Vec2 GetPosition  () const { return m_Position  ; }
            Vec2 GetDimensions() const { return m_Dimensions; }
            bool HasChanged() { if(m_DidUpdate) { m_DidUpdate = false; return true; } return false; }
        
            bool Intersects(const UIRect& Rect );
            bool Contains  (const UIRect& Rect );
            bool Contains  (const Vec2  & Point);
        
        protected:
            bool m_DidUpdate;
            Vec2 m_Position;
            Vec2 m_Dimensions;
    };
    
    class Texture;
    class UIManager;
    class Mesh;
    
    class UIElement
    {
        public:
            UIElement(UIManager* Mgr);
            ~UIElement();
        
            bool HasParent() const { return m_Parent != 0; }
            void Orphan();
            void AddChild(UIElement* E);
        
            Mesh* GetMesh() const { return m_Mesh; }
            void SetMesh(Mesh* m);
        
            void SetSize(const Vec2& Dimensions) { m_BoundingRect.Set(m_BoundingRect.GetPosition(),Dimensions); }
            void SetSize(Scalar w,Scalar h)      { m_BoundingRect.Set(m_BoundingRect.GetPosition(),Vec2(w,h) ); }
        
            void Render();
        
        protected:
            friend class UIManager;
            UID        m_ID;
            UID        m_CID;
            vector <UIElement*> m_Children;
            UIElement* m_Parent;
            UIRect     m_BoundingRect;
            Mesh*      m_Mesh;
        
            UIManager* m_Manager;
    };
};

