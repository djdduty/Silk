/*
 *  T4_Viewer.h
 *  Silk
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#pragma once
#include <Test.h>
#include <T4/Turok4.h>
#include <T4/ActorPanel.h>
#include <T4/TransformTool.h>

#define BUTTON_COLOR 0.5,0.5,0.5,1.0

namespace TestClient
{
    class T4_Viewer : public Test
    {
        public:
            T4_Viewer(i32 ArgC,char* ArgV[]);
            ~T4_Viewer();
        
            virtual void Initialize();
            void LoadMeshes();
            void LoadMaterial();
            void LoadLights();
        
            void UpdateUI();
            
            virtual void Run();
            virtual void Shutdown();
        
            bool LoadATR(const string& TurokDir,const string& File);
            void AddActorsToScene();
        
            Mat4 GetActorTransform(i32 AID) const;
            void SetActorPosition(i32 AID,const Vec3& Pos);
            void SetActorRotation(i32 AID,const Vec3& Rot);
            void SetActorScale   (i32 AID,const Vec3& Scl);
            Scalar GetActorRadius(i32 AID) const;
        
            virtual const char* GetTestName() const { return "Turok: Evolution Viewer"; }
#ifdef __APPLE__
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(1200,600); }
#else
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(1280,900); }
#endif
            virtual Renderer* GetPreferredRenderer(Rasterizer* Raster,TaskManager* TaskMng) const { return new DeferredRenderer(Raster,TaskMng); }
        
        protected:
            Turok4::ATRFile* m_ATR;
            string m_Filename;
            vector<SilkObjectVector> m_Actors;
            vector<Turok4::Actor*>   m_ActorDefs;
            vector<bool>             m_ActorIsStatic;
            Shader* m_Shdr;
        
            bool     m_ActorPanelButtonDown;
            Scalar   m_ToolbarTranslation;
            Scalar   m_ToolbarVelocity;
            bool     m_ToolbarButtonDown;
            UIPanel* m_Toolbar;
            UIPanel* m_LoadButton;
            UIPanel* m_SaveButton;
            ActorPanel* m_ActorPanel;
            TransformTool* m_TransformTool;
    };
};
