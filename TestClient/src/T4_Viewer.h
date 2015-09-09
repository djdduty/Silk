/*
 *  T4_Viewer.h
 *  Silk
 *
 *  Created by Michael DeCicco on 8/30/15.
 *
 */

#pragma once
#include <Test.h>
#include <T4/ATRLoader.h>

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
            
            virtual void Run();
            virtual void Shutdown();
        
            bool LoadATR(const string& TurokDir,const string& File);
            void AddActorToScene(Actor* a);
        
            virtual const char* GetTestName() const { return "Turok: Evolution Viewer"; }
#ifdef __APPLE__
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(800,600); }
#else
            virtual Vec2 GetPreferredInitResolution() const { return Vec2(1280,900); }
#endif
        
        protected:
            ATRFile m_ATR;
            Shader* m_Shdr;
    };
};
