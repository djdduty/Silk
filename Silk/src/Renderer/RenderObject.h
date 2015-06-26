#ifndef SILK_RENDER_OBJECT_H
#define SILK_RENDER_OBJECT_H

#include <System/SilkTypes.h>
#include <vector>

#include <Renderer/Mesh.h>
#include <Renderer/Light.h>
#include <Renderer/UniformBufferTypes.h>

#include <Renderer/Material.h>

#include <Math/Math.h>

using namespace std;

namespace Silk
{
    class Renderer;
    class ObjectList;
    class Material;
    class RasterObject;
    class Shader;
    class Rasterizer;

    enum RENDER_OBJECT_TYPE
    {
        ROT_NONE,
        ROT_MESH,
        ROT_CAMERA,
        ROT_LIGHT,
        ROT_COUNT
    };

    class RenderObject
    {
        public:
            Mesh      *   GetMesh            () { return m_Mesh                        ; }
            Light     *   GetLight           () { return m_Light                       ; }
            Material  *   GetMaterial        () { return m_Material                    ; }
            const Mat4&   GetTransform       () { return m_Uniforms->GetModelMatrix  (); }
            const Mat4&   GetNormalTransform () { return m_Uniforms->GetNormalMatrix (); }
            const Mat4&   GetTextureTransform() { return m_Uniforms->GetTextureMatrix(); }
        
            bool IsInstanced();

            void SetMesh     (Mesh    * M, Material* Mat);
            void SetLight    (Light   * L               );
            void SetMaterial (Material* Mat             );
        
            void SetTransform(Mat4 Transform);
            void SetTextureTransform(Mat4 Transform);
        
            ModelUniformSet* GetUniformSet() { return m_Uniforms; }
            void UpdateUniforms();

        protected:
            RenderObject(RENDER_OBJECT_TYPE Type, Renderer* Renderer, RasterObject* Object);
            ~RenderObject();
        
            friend class Renderer;
            friend class ObjectList;
            friend class RasterObject;
            friend class ModelUniformSet;

            RasterObject* m_Object;

            RENDER_OBJECT_TYPE     m_Type           ;
            bool                   m_Enabled        ;
            Renderer*              m_Renderer       ;
            Material*              m_Material       ;
            Mesh*                  m_Mesh           ;
            Light*                 m_Light          ;
            //TODO Camera, Light
        
            ModelUniformSet*       m_Uniforms       ;

            //List references
            i32                    m_ShaderListIndex;
            i32                    m_ListIndex      ;
            ObjectList*            m_List           ;
            i32                    m_InstanceIndex  ;
            vector<RenderObject*>* m_InstanceList   ;

        private:
            bool m_DidUpdate;
            void MarkAsUpdated();
    };


    #define SilkObjectVector vector<RenderObject*>
    class ObjectList
    {
        public:
            ObjectList() : m_MeshObjects(SilkObjectVector()), m_LightObjects(SilkObjectVector()), m_CameraObjects(SilkObjectVector())
            {};
            ~ObjectList() {}

            i32  AddObject(RenderObject* Obj);
            void RemoveObject(RenderObject* Obj);

            void Clear()
            { 
                m_MeshObjects.clear();
                m_LightObjects.clear();
                m_CameraObjects.clear();
            }

            SilkObjectVector& GetMeshList()   { return m_MeshObjects;   }
            SilkObjectVector& GetCameraList() { return m_CameraObjects; }
            SilkObjectVector& GetLightList()  { return m_LightObjects;  }
        
            i32 GetShaderCount() const { return m_ShadersUsed.size(); }
            Shader* GetShader(i32 Index) const { return m_ShadersUsed[Index]; }
            SilkObjectVector& GetShaderMeshList(i32 Index) { return m_ShaderObjects[Index]; }

        protected:
            friend class Renderer;
            SilkObjectVector m_MeshObjects;
            SilkObjectVector m_LightObjects;
            SilkObjectVector m_CameraObjects;
            
            vector<Shader*> m_ShadersUsed;
            vector<vector<RenderObject*> >m_ShaderObjects;
    };
}

#endif