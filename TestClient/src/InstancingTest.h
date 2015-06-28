/*
 *  InstancingTest.h
 *  Silk
 *
 *  Created by Michael DeCicco on 6/26/15.
 *
 */

#pragma once
#include <Test.h>
#define ObjsSize 10000
#define G 0.000000000066742

namespace TestClient
{
    class InstancingTest : public Test
    {
        public:
            InstancingTest() { }
            ~InstancingTest() { }
        
            virtual void Initialize();
            void LoadMesh();
            void GenerateShader();
            void RespawnParticle(i32 i);
            void UpdateParticle(i32 i,Scalar dt,bool SpacePressed,const Vec3& StarPos);
            virtual void Run();
            virtual void Shutdown();
        
            virtual const char* GetTestName() const { return "Instancing Test"; }
        
        protected:
            Scalar SpawnD        ;
            Scalar MinaVel       ;
            Scalar MaxaVel       ;
            Scalar MinMass       ;
            Scalar MaxMass       ;
            Scalar AngularDamping;
            Scalar LinearDamping ;
            Vec3 VelDir          ;
            Vec3 VelDirVar       ;
        
            Vec3 Velocity         [ObjsSize];
            Vec3 Acceleration     [ObjsSize];
            Vec3 Position         [ObjsSize];
        
            Vec3 RotationAxis     [ObjsSize];
            Scalar RotationAngle  [ObjsSize];
            Scalar AngularVelocity[ObjsSize];
            Scalar Mass           [ObjsSize];
            RenderObject* m_Objs  [ObjsSize];
        
            Mesh    * m_Mesh    ;
            Material* m_Material;
    };
};

