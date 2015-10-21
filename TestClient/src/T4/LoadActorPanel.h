#pragma once
#include <Silk.h>
#include <T4/Turok4.h>
#include <InputManager.h>
#include <UIElements/ListBoxPanel.h>
#include <UIElements/GridViewPanel.h>

using namespace TestClient;
using namespace Turok4;
using namespace Silk;

class ActorGridTile : public GridTile
{
    public:
        ActorGridTile(UIManager* Manager,InputManager* Input);
        virtual ~ActorGridTile();
    
        virtual void GenerateContent(const Vec2& Size);
    
        bool IsLoaded() const { return m_IsLoaded; }
        void Load(Shader* Shdr);
    
        vector<RenderObject*> m_TileObjs;   
    protected:
        friend class LoadActorPanel;
		bool m_Initialized;
        bool m_IsLoaded;
        ATRFile* m_ATR;
        string m_File;
    
        InputManager* m_Input;
        Texture* m_Texture;
        Camera* m_Camera;
        Vec2 m_Size;
};

class LoadActorPanel : public UIPanel, ListBoxEventReceiver
{
    public:
        LoadActorPanel(const Vec2& Size,UIManager* Mgr,InputManager* Input);
        ~LoadActorPanel();
    
        void Toggle();
        bool IsOpen() const { return m_Translation == 1.0f; }
    
        virtual void Update(Scalar dt);
    
        virtual void OnListSelectionChanged(i32 SelectionIndex,string SelectionName,ListBoxPanel* Sender);
    
        Shader* m_GridObjDisplayShader;
    protected:
        void ParseDirectory(const string& Dir,vector<ATRFile*>& Actors);
    
        vector<string>    m_Categories;
        vector<vector<string>> m_CategorizedActorFiles;
        vector<vector<GridView*>> m_ActorGridViews;
        vector<TabPanel*> m_GridPages;
        i32 m_CurrentGrid;
    
        Scalar m_Translation;
        Scalar m_BorderSize;
        ListBoxPanel* m_CategoryListBox;
    
        InputManager* m_InputManager;
        UIManager   * m_UIManager;
};