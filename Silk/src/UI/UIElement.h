#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Renderer/RenderObject.h>

#include <vector>
using namespace std;

namespace Silk
{
    class UIManager;
    enum UIStateType
    {
        UIS_DEFAULT,
        UIS_DISABLED,
        UIS_HOVER,
        UIS_ACTIVE,
        UIS_FOCUS,
        UIS_COUNT
    };

    class UIRect //Just a Bounding Box
    {
        public:
            UIRect() : m_DidUpdate(true) { }
            UIRect(const Vec2& Pos,const Vec2& Size)    { Set(Pos,Size); }
            UIRect(Scalar x,Scalar y,Scalar w,Scalar h) { Set(x,y,w,h); }
            ~UIRect() { }
        
            void Set(const Vec2& Pos,const Vec2& Size)    { m_Position = Pos      ; m_Dimensions = Size     ; m_DidUpdate = true; }
            void Set(Scalar x,Scalar y,Scalar w,Scalar h) { m_Position = Vec2(x,y); m_Dimensions = Vec2(w,h); m_DidUpdate = true; }
            void SetPosition(const Vec2& Pos)             { m_Position = Pos  ; m_DidUpdate = true; }
            void SetDimensions(const Vec2& Dim)           { m_Dimensions = Dim; m_DidUpdate = true; }
        
            Vec2 GetPosition  () const { return m_Position  ; }
            Vec2 GetDimensions() const { return m_Dimensions; }
        
            bool Intersects(const UIRect& Rect );
            bool Contains  (const UIRect& Rect );
            bool Contains  (const Vec2  & Point);
        
        protected:
            bool m_DidUpdate;
            Vec2 m_Position;
            Vec2 m_Dimensions;
    };
    
    class UIElement
    {
        public:
            UIElement();
        
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            i32 Destroy();
        
            bool HasParent() const { return m_Parent != 0; }
            void Orphan();
            void AddChild(UIElement* E);
            void RemoveChild(UIElement* E);
            UIElement* GetParent() { return m_Parent; }

            virtual void Update(Scalar dt) { }
            void _PreRender();
            virtual void PreRender() {}
            virtual void Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);
            void _PostRender();
            virtual void PostRender() {}
            
            virtual void OnMouseMove() {}
            virtual void OnMouseOver() {}
            virtual void OnMouseDown() {}
            virtual void OnMouseOut()  {}
            virtual void OnMouseUp()   {}
            virtual void OnKeyDown()   {}
            virtual void OnKeyHeld()   {}
            virtual void OnKeyUp()     {}

            void SetPosition(Vec3 Pos);
            void SetSize(Vec2 Size);
            Vec3 GetPosition() { return m_Render->GetTransform().GetTranslation(); };
            UIRect* GetBounds()   const { return m_Bounds; }
            Vec2 GetChildOffset() const { return m_ChildOffset; }

            void    EnableScissor(bool Enable) { m_ScissorEnabled = Enable; }
            void    UpdateOuterBounds();
            UIRect* GetOuterBounds() const { return m_OuterBounds; }
            void    SetChildOffset(Vec2 Off);

        protected:
            friend class UIManager;
            friend class UIElement;
			friend class UIText;

            virtual void OnInitialize() {}

            virtual            ~UIElement    ();
            void               _Initialize   (UIManager* Manager);
            void               _Update(Scalar dt);
            void               _Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);
            
            i32                m_RefCount;
            UID                m_ID;
            UID                m_CID;
            UIRect*            m_Bounds;
            UIRect*            m_OuterBounds;
            RenderObject*      m_Render;
            Material*          m_Material;
            Vec2               m_ChildOffset;
            bool               m_ScissorEnabled;
            bool               m_MeshNeedsUpdate;
        
            vector<UIElement*> m_Children;
            UIElement*         m_Parent;
            UIManager*         m_Manager;
            bool               m_Initialized;
    };
};

