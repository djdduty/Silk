#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <Renderer/Renderer.h>
#include <Renderer/UniformBufferTypes.h>
#include <Renderer/RenderObject.h>

#include <UI/UIContent.h>

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

    class UIElement;
    class UIElementStyle
    {
        public:
            UIElementStyle (UIElement* Parent) : m_Parent(Parent), m_Size(Vec2(0,0)), m_BackgroundOffset(Vec2(0,0)), m_BackgroundSize(Vec2(0,0)),
                            m_BackgroundImage(0), m_BorderRadius(0), m_BorderWidth(0), m_BackgroundColor(Vec4(0,0,0,0)), m_BorderColor(Vec4(0,0,0,0)), 
                            m_TextColor(Vec4(0,0,0,0)), m_TextSize(14), m_Position(Vec3(0,0,0)) {}
            ~UIElementStyle() {}

            void BroadcastMeshChange();
            void BroadcastTransformChange();
            void BroadcastMaterialChange();
            void BroadcastContentChange();

            void SetPosition        (Vec3     Value) { m_Position         = Value; BroadcastTransformChange(); }
            void SetSize            (Vec2     Value) { m_Size             = Value; BroadcastMeshChange();      }
            void SetBackgroundOffset(Vec2     Value) { m_BackgroundOffset = Value; BroadcastMeshChange();      } // Tex Coords
            void SetBackgroundSize  (Vec2     Value) { m_BackgroundSize   = Value; BroadcastMeshChange();      } // Tex Coords
            void SetBackgroundImage (Texture* Value) { m_BackgroundImage  = Value; BroadcastMaterialChange();  }
            void SetBorderRadius    (f32      Value) { m_BorderRadius     = Value; BroadcastMeshChange();      }
            void SetBorderWidth     (f32      Value) { m_BorderWidth      = Value; BroadcastMeshChange();      }
            void SetBackgroundColor (Vec4     Value) { m_BackgroundColor  = Value; BroadcastMaterialChange();  }
            void SetBorderColor     (Vec4     Value) { m_BorderColor      = Value; BroadcastMaterialChange();  }
            void SetTextColor       (Vec4     Value) { m_TextColor        = Value; BroadcastMaterialChange();  }
            void SetTextSize        (f32      Value) { m_TextSize         = Value; BroadcastContentChange();   }

            Vec3     GetPosition        () const { return m_Position;         }
            Vec2     GetSize            () const { return m_Size;             }
            Vec2     GetBackgroundOffset() const { return m_BackgroundOffset; }
            Vec2     GetBackgroundSize  () const { return m_BackgroundSize;   }
            Texture* GetBackgroundImage () const { return m_BackgroundImage;  }
            f32      GetBorderRadius    () const { return m_BorderRadius;     }
            f32      GetBorderWidth     () const { return m_BorderWidth;      }
            Vec4     GetBackgroundColor () const { return m_BackgroundColor;  }
            Vec4     GetBorderColor     () const { return m_BorderColor;      }
            Vec4     GetTextColor       () const { return m_TextColor;        }
            f32      GetTextSize        () const { return m_TextSize;         }

        protected:
            //TODO(?): Parent Style, For a class-esque sort of implementation. The parent would override everything but our size and position
            UIElement* m_Parent;

            Vec3     m_Position         ;
            Vec2     m_Size             ;
            Vec2     m_BackgroundOffset ;
            Vec2     m_BackgroundSize   ;
            Texture* m_BackgroundImage  ;
            f32      m_BorderRadius     ;
            f32      m_BorderWidth      ;
            Vec4     m_BackgroundColor  ;
            Vec4     m_BorderColor      ;
            Vec4     m_TextColor        ;
            f32      m_TextSize         ;
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
    
    class UIElement
    {
        public:
            UIElement();
        
            i32 AddRef() { m_RefCount++; return m_RefCount; }
            i32 Destroy();
        
            bool HasParent() const { return m_Parent != 0; }
            void Orphan();
            void AddChild(UIElement* E);

            void SetState(UIStateType State) 
            { 
                m_CurrentState = State; 
            }
            UIElementStyle* GetCurrentStyle() const
            { 
                if(m_States[m_CurrentState]) 
                    return m_States[m_CurrentState]; 
                return m_States[UIS_DEFAULT]; 
            }
            UIElementStyle* GetStyle(UIStateType State)
            {
                if(m_States[State])
                    return m_States[State];
                m_States[State] = new UIElementStyle(this);
                return m_States[State];
            }

            void AddContent(UIRenderContent* Content);
            void RemoveContent(UIRenderContent* Content);

            virtual void Update(Scalar dt) { }
            void UpdateTransforms();
            void UpdateMaterials();
            void UpdateMeshes();
            void UpdateContent();
            
            virtual void OnUpdateTransforms() {}
            virtual void OnUpdateMaterials()  {}
            virtual void OnUpdateMeshes()     {}
            virtual void OnUpdateContent()    {}

            void Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);
        
        protected:
            friend class UIElementStyle;
            friend class UIManager;

            virtual      ~UIElement    ();
            virtual void _Initialize   ();
            void         _Update(Scalar dt);
            void         _Render(PRIMITIVE_TYPE PrimType, SilkObjectVector* ObjectsRendered);
            
            i32                m_RefCount;
            UID                m_ID;
            UID                m_CID;
            UIElement*         m_Parent;
            bool               m_MeshNeedsUpdate;
            bool               m_TransformNeedsUpdate;
            bool               m_MaterialNeedsUpdate;
            bool               m_ContentNeedsUpdate;

            std::vector<UIRenderContent*> m_Content;

            UIStateType        m_CurrentState;
            UIElementStyle*    m_States[UIS_COUNT];
        
            vector<UIElement*> m_Children;
            UIManager* m_Manager;
    };
};

