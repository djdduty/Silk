#include <T4/LoadActorPanel.h>
#include <T4/ConvertActor.h>
#include <T4/Directory.h>
#include <UIElements/Common.h>
#include <GLFW/glfw3.h>

ActorGridTile::ActorGridTile(UIManager* Mgr,InputManager* Input) : m_Input(Input), m_Texture(0), m_ATR(0), m_IsLoaded(false)
{
    m_Camera = new Camera();
	m_Initialized = false;
}
ActorGridTile::~ActorGridTile()
{
    if(m_Texture) m_Texture->Destroy();
    if(m_ATR) delete m_ATR;
}

void ActorGridTile::GenerateContent(const Vec2& Size)
{
    if(m_Size.x != Size.x || m_Size.y != Size.y)
    {
        if(!m_Texture) m_Texture = m_Manager->GetRenderer()->GetRasterizer()->CreateTexture();
        m_Texture->CreateTextureb(Size.x,Size.y,0);
        m_Texture->UpdateTexture();
        m_Size = Size;
    }
    if(m_TileObjs.size() == 0 || !m_Texture) { return; }
    
    Vec3 MaxExtents = m_TileObjs[0]->GetBoundingBox().GetLocalAABB().GetExtents();
    ObjectList l;
    for(i32 i = 0;i < m_TileObjs.size();i++)
    {
        l.AddObject(m_TileObjs[i]);
        Vec3 Extents = m_TileObjs[i]->GetBoundingBox().GetLocalAABB().GetExtents();
        if(MaxExtents.x < Extents.x) MaxExtents.x = Extents.x;
        if(MaxExtents.y < Extents.y) MaxExtents.y = Extents.y;
        if(MaxExtents.z < Extents.z) MaxExtents.z = Extents.z;
        m_TileObjs[i]->SetEnabled(true);
		
		if(!m_Initialized)
		{
			m_TileObjs[i]->SetTransform(Translation(-m_TileObjs[i]->GetBoundingBox().GetLocalAABB().GetCenter()));
		}
    }
	m_Initialized = true;
    
    
    Scalar Aspect = Size.y / Size.x;
    m_Camera->SetFieldOfView(Vec2(60.0f,60.0f * Aspect));
    
    m_Camera->SetTransform(Translation(Vec3(0,1,1).Normalized() * MaxExtents.Magnitude() * 1.0f) * RotationX(45.0f));
    
	Vec4 bgc = m_Manager->GetRenderer()->GetRasterizer()->GetClearColor();
	m_Manager->GetRenderer()->GetRasterizer()->SetClearColor(Vec4(0.55f,0.55f,0.55f,1.0f));
    m_Manager->GetRenderer()->RenderObjectsToTexture(&l,m_Texture,m_Camera);
	m_Manager->GetRenderer()->GetRasterizer()->SetClearColor(bgc);
    
	SetBackgroundColor(Vec4(0.55f,0.55f,0.55f,1.0f));
    SetBackgroundImage(m_Texture);
    
    for(i32 i = 0;i < m_TileObjs.size();i++) m_TileObjs[i]->SetEnabled(false);
	SetNeedsUpdate();
	RaiseViewUpdatedFlag();
}
void ActorGridTile::Load(Shader* Shdr)
{
    m_IsLoaded = true;
    m_ATR = new ATRFile();
    
    if(!m_ATR->Load(TransformPseudoPathToRealPath(m_File))) { delete m_ATR; m_ATR = 0; return; }
    
    LoadActorToEngine(m_Manager->GetRenderer(),Shdr,Mat4::Identity,0,m_ATR->GetActor(),m_TileObjs);
}

bool stringCompare( const string &left, const string &right )
{
   for( string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit )
      if( tolower( *lit ) < tolower( *rit ) )
         return true;
      else if( tolower( *lit ) > tolower( *rit ) )
         return false;
   if( left.size() < right.size() )
      return true;
   return false;
}

LoadActorPanel::LoadActorPanel(const Vec2& Size,UIManager* Mgr,InputManager* Input) : UIPanel(Size), m_InputManager(Input), m_UIManager(Mgr)
{
    FILE* fp = fopen("T4/ActorCache.dat","rb");
    
    if(!fp)
    {
        vector<ATRFile*> Actors;
        ParseDirectory(GetTurokDirectory() + "/data/actors",Actors);
        
        //Categorize the actors by "ACTOR_CODE"
        for(i32 i = 0;i < Actors.size();i++)
        {
            string Code = Actors[i]->GetActorCode();
            
            //Find out if category exists
            bool Exists = false;
            for(i32 c = 0;c < m_Categories.size();c++) { if(m_Categories[c] == Code) { Exists = true; break; } }
            
            if(!Exists)
            {
                //Create category
                m_Categories.push_back(Code);
                m_CategorizedActorFiles.push_back(vector<string>());
                
                //Add actor to category
                m_CategorizedActorFiles[m_CategorizedActorFiles.size() - 1].push_back(Actors[i]->GetActor()->GetFilename());
            }
        }
        
        sort(m_Categories.begin(),m_Categories.end(),stringCompare);
        
        for(i32 i = 0;i < Actors.size();i++)
        {
            string Code = Actors[i]->GetActorCode();
            
            //Find out if category exists
            bool Exists = false;
            for(i32 c = 0;c < m_Categories.size();c++)
            {
                if(m_Categories[c] == Code) m_CategorizedActorFiles[c].push_back(Actors[i]->GetActor()->GetFilename());
            }
        }
        
        for(i32 c = 0;c < m_Categories.size();c++) sort(m_CategorizedActorFiles[c].begin(),m_CategorizedActorFiles[c].end(),stringCompare);
        
        ByteStream* CacheFile = new ByteStream();
        
        CacheFile->WriteInt32(m_Categories.size());
        for(i32 i = 0;i < m_Categories.size();i++)
        {
            CacheFile->WriteString(m_Categories[i]);
            CacheFile->WriteInt32(m_CategorizedActorFiles[i].size());
            for(i32 a = 0;a < m_CategorizedActorFiles[i].size();a++)
            {
                CacheFile->WriteString(m_CategorizedActorFiles[i][a]);
            }
        }
        
        fp = fopen("T4/ActorCache.dat","wb");
        if(!fp) printf("Unable to open ActorCache.dat for writing.\n");
        else
        {
            CacheFile->SetOffset(0);
            if(fwrite(CacheFile->Ptr(),CacheFile->GetSize(),1,fp) != 1)
            {
                printf("Unable to save categorized actor cache file.\n");
                fclose(fp);
            }
            fclose(fp);
        }
        
        delete CacheFile;
    }
    else
    {
        ByteStream* CacheFile = new ByteStream(fp);
        
        i32 CategoryCount = CacheFile->GetInt32();
        for(i32 i = 0;i < CategoryCount;i++)
        {
            string Name = CacheFile->GetString();
            m_Categories.push_back(Name);
            m_CategorizedActorFiles.push_back(vector<string>());
            
            i32 CategoryActorCount = CacheFile->GetInt32();
            for(i32 a = 0;a < CategoryActorCount;a++)
            {
                string File = CacheFile->GetString();
                
                m_CategorizedActorFiles[i].push_back(File);
            }
        }
        
        delete CacheFile;
    }
    
    ShaderGenerator* Gen = Mgr->GetRenderer()->GetShaderGenerator();
    
    Gen->Reset();
    Gen->SetShaderVersion   (330);
    Gen->SetAllowInstancing (false);
    
    Gen->SetUniformInput    (ShaderGenerator::IUT_RENDERER_UNIFORMS,true);
    Gen->SetUniformInput    (ShaderGenerator::IUT_MATERIAL_UNIFORMS,true);
    Gen->SetUniformInput    (ShaderGenerator::IUT_OBJECT_UNIFORMS  ,true);
    
    Gen->SetTextureInput    (Material::MT_DIFFUSE, true);
    
    Gen->SetAttributeInput  (ShaderGenerator::IAT_POSITION,true);
    Gen->SetAttributeInput  (ShaderGenerator::IAT_TEXCOORD,true);
    
    Gen->SetAttributeOutput (ShaderGenerator::IAT_POSITION,true);
    Gen->SetAttributeOutput (ShaderGenerator::IAT_TEXCOORD,true);
    
    Gen->SetFragmentOutput  (ShaderGenerator::OFT_COLOR   ,true);
    Gen->AddFragmentModule("[AlphaTest]if(sColor.a < 0.01) discard;[/AlphaTest]",0);
    Gen->SetLightingMode(ShaderGenerator::LM_FLAT);
    m_GridObjDisplayShader = Gen->Generate();
    
    SetBackgroundColor(Vec4(0.7f,0.7f,0.7f,1.0f));
    m_BorderSize = 2.0f;
    Mgr->AddElement(this);
    
    m_CategoryListBox = new ListBoxPanel(Vec2(Size.x * 0.2f,Size.y - (2.0f * m_BorderSize)),Input,this);
    m_CategoryListBox->SetPosition(Vec3(Size.x - m_BorderSize - (Size.x * 0.2f),m_BorderSize,0.0f));
    m_CategoryListBox->SetReceiver(this);
    m_CurrentGrid = 0;
    
    for(i32 i = 0; i < m_Categories.size();i++)
    {
        m_CategoryListBox->AddEntry(m_Categories[i]);
        m_ActorGridViews.push_back(vector<GridView*>());
		m_SelectedPageIndices.push_back(0);
        m_GridPages.push_back(0);
    }
    OnListSelectionChanged(0,"",0);
}
LoadActorPanel::~LoadActorPanel()
{
}
void LoadActorPanel::ParseDirectory(const string &Dir,vector<ATRFile*>& Actors)
{
    DirectoryInfo DirInfo;
    DirInfo.Populate(Dir);
    
    for(i32 i = 0;i < DirInfo.GetEntryCount();i++)
    {
        string Name = DirInfo.GetEntryName(i);
        if(Name == "." || Name == "..") continue;
        
        if(DirInfo.GetEntryExtension(i) == "atr")
        {
            ATRFile* f = new ATRFile();
            if(f->Load(Dir + "/" + Name)) Actors.push_back(f);
            else { delete f; printf("Failed to load actor <%s>.\n",Name.c_str()); }
        }
        else if(DirInfo.GetEntryType(i) == DET_FOLDER) ParseDirectory(Dir + "/" + DirInfo.GetEntryName(i),Actors);
    }
}
void LoadActorPanel::Toggle()
{
}
void LoadActorPanel::Update(Scalar dt)
{
	if(!WillRender()) return;
    UIPanel::Update(dt);
    
	vector<GridTile*> Tiles;
	i32 SelectedTab = m_GridPages[m_CurrentGrid]->GetSelectedTabIndex();
	if(m_SelectedPageIndices[m_CurrentGrid] != SelectedTab)
	{
		OnPageChanged(m_SelectedPageIndices[m_CurrentGrid],SelectedTab); 
		m_SelectedPageIndices[m_CurrentGrid] = SelectedTab;
	}
	GridView* Grid = m_ActorGridViews[m_CurrentGrid][SelectedTab];
	Grid->SetNeedsUpdate();
	Grid->GetVisibleTiles(Tiles);
    for(i32 i = 0;i < Tiles.size();i++)
    {
        ActorGridTile* t = (ActorGridTile*)Tiles[i];
        
        if(!t->IsLoaded()) t->Load(m_GridObjDisplayShader);
        
        for(i32 a = 0;a < t->m_TileObjs.size();a++)
        {
            t->m_TileObjs[a]->SetTransform(t->m_TileObjs[a]->GetTransform() * RotationY(15.0f * dt));
        }
        
        t->GenerateContent(t->m_Size);
    }
}
void LoadActorPanel::OnListSelectionChanged(i32 SelectionIndex,string SelectionName,ListBoxPanel* Sender)
{
    if(m_CurrentGrid > 0 && m_CurrentGrid < m_ActorGridViews.size() && m_ActorGridViews[m_CurrentGrid].size() > 0)
    {
        m_GridPages     [m_CurrentGrid]->SetEnabled(false);
    }
    m_CurrentGrid = SelectionIndex;
    
    if(m_ActorGridViews[m_CurrentGrid].size() == 0)
    {
        if(m_CategorizedActorFiles[m_CurrentGrid].size() == 0) return;
        
        TabPanel* Pages = new TabPanel(Vec2(0,0),m_InputManager);
		m_GridPages[m_CurrentGrid] = Pages;
        AddChild(Pages);
        Pages->SetPosition(Vec3(m_BorderSize,m_BorderSize,0.0f));
        Pages->SetSize(Vec2(m_CategoryListBox->GetPosition().x - (2.0f * m_BorderSize),m_Bounds->GetDimensions().y - (2.0f * m_BorderSize)));
        
        m_GridPages[m_CurrentGrid] = Pages;

		i32 PageCount = (m_CategorizedActorFiles[m_CurrentGrid].size() / TILES_PER_PAGE) + 1;
	    for(i32 i = 0;i < PageCount;i++) 
	    {
			Pages->AddTab(FormatString("Page %d",i + 1));
			Pages->GetTabView(i)->SetAutoUpdateChildren(false);
            m_ActorGridViews[m_CurrentGrid].push_back(0);
		}
    }
    m_GridPages     [m_CurrentGrid]->SetEnabled(true);
	OnPageChanged(0,0);
}
void LoadActorPanel::OnPageChanged(i32 Old,i32 New)
{
	TabPanel* Pages = m_GridPages[m_CurrentGrid];
	GridView* Grid = new GridView(Vec2(0,0),m_InputManager,Pages->GetTabView(New));
    Grid->SetBackgroundColor(Vec4(0.6f,0.6f,0.6f,1.0f));
    Grid->SetPosition(Vec3(m_BorderSize,m_BorderSize,0.0f));
    Grid->SetSize(Pages->GetTabView(New)->GetBounds()->GetDimensions() - (Vec2(m_BorderSize,m_BorderSize) * 2.0f));
    Grid->SetTilesPerRow(6);
    Grid->SetTileSpacing(4.0f);
            
	for(i32 a = New * TILES_PER_PAGE;a < m_CategorizedActorFiles[m_CurrentGrid].size() && a < ((New * TILES_PER_PAGE) + TILES_PER_PAGE);a++)
    {
        ActorGridTile* Tile = new ActorGridTile(m_Manager,m_InputManager);
        Tile->SetBackgroundColor(Vec4(0.55f,0.55f,0.55f,1.0f));
                
        Grid->AddTile(Tile);
        
        string f = m_CategorizedActorFiles[m_CurrentGrid][a].substr(m_CategorizedActorFiles[m_CurrentGrid][a].find_last_of('\\') + 1,
                                                                    m_CategorizedActorFiles[m_CurrentGrid][a].length() - 1);
        Tile->m_File = m_CategorizedActorFiles[m_CurrentGrid][a];
        AddText(m_Manager,16.0f,Vec2(2,2),f,Tile);
        AddText(m_Manager,24.0f,Vec2(2,Grid->GetTileSize() - 26.0f),FormatString("%d",a + 1),Tile);
    }
            
    m_ActorGridViews[m_CurrentGrid][New] = Grid;
}









