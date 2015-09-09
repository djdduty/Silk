#include <Utilities/DebugDrawer.h>
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>

namespace Silk
{
    DebugDrawer::DebugDrawer(Renderer* r) : m_Renderer(r)
    {
        m_Material = r->CreateMaterial();
        
        ShaderGenerator* g = r->GetShaderGenerator();
        g->Reset();
        g->SetShaderVersion(330);
        g->SetAllowInstancing(false);
        g->SetUniformInput(ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
        g->SetUniformInput(ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
        
        g->SetAttributeInput (ShaderGenerator::IAT_POSITION,true);
        g->SetAttributeInput (ShaderGenerator::IAT_COLOR   ,true);
        g->SetAttributeOutput(ShaderGenerator::IAT_COLOR   ,true);
        g->SetFragmentOutput (ShaderGenerator::OFT_COLOR   ,true);
        
        g->SetLightingMode(ShaderGenerator::LM_FLAT);
        
        Shader* Shdr = g->Generate();
        m_Material->SetShader(Shdr);
        Shdr->Destroy();
        
        m_Display = m_Renderer->CreateRenderObject(ROT_MESH);
        Mesh* m = new Mesh();
        m_Display->SetMesh(m,m_Material);
        m->Destroy();
        
        m_Renderer->GetScene()->AddRenderObject(m_Display);
    }
    DebugDrawer::~DebugDrawer()
    {
    }

    void DebugDrawer::Line(const Vec3& a,const Vec3& b,const Vec4& Color)
    {
        AddVertex(a,Color);
        AddVertex(b,Color);
    }
    void DebugDrawer::Transform(const Mat4& t,const Vec4& ColorX,const Vec4& ColorY,const Vec4& ColorZ,const Vec3& Scale)
    {
        Vec3 Origin = t.GetTranslation();
        Line(Origin,Origin + (t.GetX() * Scale.x),ColorX);
        Line(Origin,Origin + (t.GetY() * Scale.y),ColorY);
        Line(Origin,Origin + (t.GetZ() * Scale.z),ColorZ);
    }
    void DebugDrawer::Box(const Mat4& t,const Vec3& HalfExtents,const Vec4& Color)
    {
        
        /* 
         *       1-------3
         *      /|      /|
         *     5_______7 |
         *     | 0_____|_2
         *     |/      |/
         *     4_______6
         *
         */
        Mat4 tt = t.Transpose();
        Vec3 Box[8] =
        {
            tt * (Vec3(-1.0f,-1.0f, 1.0f) * HalfExtents),
            tt * (Vec3(-1.0f, 1.0f, 1.0f) * HalfExtents),
            tt * (Vec3( 1.0f,-1.0f, 1.0f) * HalfExtents),
            tt * (Vec3( 1.0f, 1.0f, 1.0f) * HalfExtents),
            
            tt * (Vec3(-1.0f,-1.0f,-1.0f) * HalfExtents),
            tt * (Vec3(-1.0f, 1.0f,-1.0f) * HalfExtents),
            tt * (Vec3( 1.0f,-1.0f,-1.0f) * HalfExtents),
            tt * (Vec3( 1.0f, 1.0f,-1.0f) * HalfExtents),
        };
        
        Line(Box[0],Box[1],Color);
        Line(Box[2],Box[3],Color);
        Line(Box[1],Box[3],Color);
        Line(Box[0],Box[2],Color);
        
        Line(Box[4],Box[5],Color);
        Line(Box[6],Box[7],Color);
        Line(Box[4],Box[6],Color);
        Line(Box[5],Box[7],Color);
        
        Line(Box[4],Box[0],Color);
        Line(Box[6],Box[2],Color);
        Line(Box[5],Box[1],Color);
        Line(Box[7],Box[3],Color);
    }
    void DebugDrawer::OBB(const Silk::OBB &Box,const Vec4& Color)
    {
		DebugDrawer::Box(Box.GetObject()->GetTransform() * Translation((Box.GetLocalAABB().GetCenter())),Box.GetLocalAABB().GetExtents(),Color);
    }
    void DebugDrawer::AABB(const Mat4& ObjTrans,const Silk::AABB& Box,const Vec4& Color)
    {
        DebugDrawer::Box(Translation((Box.GetCenter())),Box.GetExtents(),Color);
    }
	void DebugDrawer::DrawCamera(Camera* Cam,const Vec4& Color)
	{
		/* 
         *       1-------3
         *      /|      /|
         *     5_______7 |
         *     | 0_____|_2
         *     |/      |/
         *     4_______6
         *
         */
		Mat4 tt = (Cam->GetTransform() * Cam->GetProjection().Inverse()).Transpose();
		Vec3 HalfExtents(1.0,1.0,1.0);
        Vec3 Box[8] =
        {
            tt * (Vec3(-1.0f,-1.0f, 1.0f) * HalfExtents),
            tt * (Vec3(-1.0f, 1.0f, 1.0f) * HalfExtents),
            tt * (Vec3( 1.0f,-1.0f, 1.0f) * HalfExtents),
            tt * (Vec3( 1.0f, 1.0f, 1.0f) * HalfExtents),
            
            tt * (Vec3(-1.0f,-1.0f,-1.0f) * HalfExtents),
            tt * (Vec3(-1.0f, 1.0f,-1.0f) * HalfExtents),
            tt * (Vec3( 1.0f,-1.0f,-1.0f) * HalfExtents),
            tt * (Vec3( 1.0f, 1.0f,-1.0f) * HalfExtents),
        };

        Line(Box[0],Box[1],Color);
        Line(Box[2],Box[3],Color);
        Line(Box[1],Box[3],Color);
        Line(Box[0],Box[2],Color);
        
        Line(Box[4],Box[5],Color);
        Line(Box[6],Box[7],Color);
        Line(Box[4],Box[6],Color);
        Line(Box[5],Box[7],Color);
        
        Line(Box[4],Box[0],Color);
        Line(Box[6],Box[2],Color);
        Line(Box[5],Box[1],Color);
        Line(Box[7],Box[3],Color);
	}
    void DebugDrawer::AddVertex(const Vec3 &Vert,const Vec4 &Color)
    {
        m_Verts.push_back(Vert);
        m_Colors.push_back(Color);
    }

    void DebugDrawer::Update(Scalar dt)
    {
		if(m_Verts.size() == 0) return; 

        Mesh* m = new Mesh();
        m->PrimitiveType = PT_LINES;
        m->SetVertexBuffer(m_Verts .size(),&m_Verts [0].x);
        m->SetColorBuffer (m_Colors.size(),&m_Colors[0].x);
        m_Display->SetMesh(m,m_Material);
        m->Destroy();
        m_Verts .clear();
        m_Colors.clear();
    }
};
