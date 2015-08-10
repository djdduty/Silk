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
    class RenderObject;
    
    class UIElement
    {
        public:
            UIElement();
        
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            i32 Destroy();
        
            bool HasParent() const { return m_Parent != 0; }
            void Orphan();
            void AddChild(UIElement* E);
        
            RenderObject* GetObject() const { return m_Render; }
            void SetObject(RenderObject* o);
        
            void SetSize(const Vec2& Dimensions) { m_Dimensions = Dimensions; }
            void SetSize(Scalar w,Scalar h)      { m_Dimensions = Vec2(w,h) ; }
        
            UIRect GetArea() const;
        
            virtual void Update(Scalar dt) { }
        
        protected:
            friend class UIManager;
            virtual ~UIElement();
            void _Update(Scalar dt);
            
            i32           m_RefCount;
            UID           m_ID;
            UID           m_CID;
            UIElement*    m_Parent;
            Vec2          m_Dimensions;
            RenderObject* m_Render;
        
            vector <UIElement*> m_Children;
            UIManager* m_Manager;
    };
};

