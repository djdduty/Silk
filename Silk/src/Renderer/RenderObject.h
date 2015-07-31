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
        
            bool IsVisible() const { return !m_IsCulled; }
        
            bool IsInstanced();
            i32 GetInstanceIndex() const { return m_InstanceIndex; }
            i32 GetCulledInstanceIndex() const { return m_CulledInstanceIndex; }
            RasterObject* GetObject() const { return m_Object; }

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
        
            friend class Scene;
            friend class Renderer;
            friend class ObjectList;
            friend class RasterObject;
            friend class ModelUniformSet;
            friend class CullingAlgorithm;

            RasterObject* m_Object;

            RENDER_OBJECT_TYPE     m_Type           ;
            bool                   m_Enabled        ;
            bool                   m_IsCulled       ;
            Renderer*              m_Renderer       ;
            Material*              m_Material       ;
            Mesh*                  m_Mesh           ;
            Light*                 m_Light          ;
            //TODO Camera
        
            ModelUniformSet*       m_Uniforms       ;

            //List references
            i32                    m_ShaderListIndex;
            i32                    m_MeshListIndex  ;
            i32                    m_ListIndex      ;
            ObjectList*            m_List           ;
            vector<RenderObject*>* m_InstanceList   ;
            i32                    m_InstanceIndex  ;
            i32                    m_CulledInstanceIndex;

        private:
            bool m_DidUpdate;
            void MarkAsUpdated();
    };


    #define SilkObjectVector vector<RenderObject*>
    class ObjectList
    {
        public:
            ObjectList() : m_IsIndexed(true), m_MeshObjects(SilkObjectVector()), m_LightObjects(SilkObjectVector()), m_CameraObjects(SilkObjectVector())
            { }
            ObjectList(const ObjectList& l);
            ~ObjectList() { }

            void SetIndexed(bool Flag) { m_IsIndexed = Flag; }
            bool IsIndexed() const { return m_IsIndexed; }
        
            i32  AddObject(RenderObject* Obj);
            void RemoveObject(RenderObject* Obj);

            void Clear();

            vector<Mesh*>* GetSceneMeshes()   { return &m_Meshes      ; }
            SilkObjectVector& GetMeshList()   { return m_MeshObjects  ; }
            SilkObjectVector& GetCameraList() { return m_CameraObjects; }
            SilkObjectVector& GetLightList()  { return m_LightObjects ; }
        
            i32 GetShaderCount() const { return m_ShadersUsed.size(); }
            Shader* GetShader(i32 Index) const { return m_ShadersUsed[Index]; }
            SilkObjectVector& GetShaderMeshList(i32 Index) { return m_ObjectsByShader[Index]; }

        protected:
            friend class Renderer;
            bool m_IsIndexed;
        
            SilkObjectVector m_MeshObjects;
            SilkObjectVector m_LightObjects;
            SilkObjectVector m_CameraObjects;
            
            vector<Shader*> m_ShadersUsed;
            vector<vector<RenderObject*> >m_ObjectsByShader;
            vector<vector<RenderObject*> >m_ObjectsByMesh;
            vector<Mesh*> m_Meshes;
    };
}

#endif