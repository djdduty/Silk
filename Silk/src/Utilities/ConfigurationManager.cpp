#include <Utilities/ConfigurationManager.h>

#define CONFIG_HEADER {'c','f','g',0}
#define NODE_BEGIN_BLOCK_ID     0xBBBB0001
#define NODE_END_BLOCK_ID       0xBBBB0002
#define CONFIG_FILE_END         0xBBBB0003

namespace Silk
{
    ConfigNode::~ConfigNode()
    {
        if(m_Value) delete m_Value;
        for(i32 i = 0;i < m_Children.size();i++) delete m_Children[i];
        m_Children.clear();
    }
    
    void ConfigNode::Initialize(ConfigValue::VALUE_TYPE Type,void *Ptr)
    {
        if(m_Value) delete m_Value;
        m_Value = new ConfigValue(Type,Ptr);
    }
    ConfigNode* ConfigNode::AddNode(const string &Name)
    {
        ConfigNode* n = new ConfigNode(Name);
        m_Children.push_back(n);
        return n;
    }
    size_t ConfigNode::Serialize(Byte **OutDataPtr)
    {
        /* Build virtual file structure */
        size_t TotalSize = 0;
        vector<Byte*> Ptrs;
        vector<size_t> Sizes;
        
        TotalSize += sizeof(i32 );                   //Node block ID
        TotalSize += sizeof(i32 );                   //Node name length
        TotalSize += sizeof(Byte) * m_Name.length(); //Node name
        TotalSize += sizeof(bool);                   //Node contains value (bool)
        TotalSize += sizeof(i32 );                   //Child count
        
        //Value
        if(m_Value)
        {
            Byte* ptr = 0;
            Sizes.push_back(m_Value->Serialize(&ptr));
            Ptrs.push_back(ptr);
            TotalSize += Sizes[Sizes.size() - 1];
        }
        else { Sizes.push_back(0); Ptrs.push_back(0); }
        
        //Children
        for(i32 i = 0;i < m_Children.size();i++)
        {
            Byte* ptr = 0;
            Sizes.push_back(m_Children[i]->Serialize(&ptr));
            Ptrs.push_back(ptr);
            TotalSize += Sizes[Sizes.size() - 1];
        }
        
        TotalSize += sizeof(i32); //Node end block ID
        
        /* Assemble data */
        Byte* AssembledData = (*OutDataPtr) = new Byte[TotalSize];
        
        i32 nbbid = NODE_BEGIN_BLOCK_ID;
        i32 nebid = NODE_END_BLOCK_ID  ;
        i32 NmLen = m_Name.length()    ;
        bool ContainsVal = m_Value != 0;
        i32 CConut  = m_Children.size();
        
        memcpy(AssembledData,&nbbid      ,sizeof(i32 )        ); AssembledData +=  sizeof(i32 );          //Begin node block
        memcpy(AssembledData,&NmLen      ,sizeof(i32 )        ); AssembledData +=  sizeof(i32 );          //Name length
        memcpy(AssembledData,&m_Name[0]  ,sizeof(char) * NmLen); AssembledData += (sizeof(char) * NmLen); //Name
        memcpy(AssembledData,&ContainsVal,sizeof(bool)        ); AssembledData +=  sizeof(bool);          //Block contains value?
        memcpy(AssembledData,&CConut     ,sizeof(i32 )        ); AssembledData +=  sizeof(i32 );          //Child count
        
        //Value
        if(m_Value) { memcpy(AssembledData,Ptrs[0],Sizes[0]); AssembledData += Sizes[0]; delete Ptrs[0]; }
        
        //Children
        for(i32 i = 1;i < Ptrs.size();i++)
        {
            memcpy(AssembledData,Ptrs[i],Sizes[i]);
            AssembledData += Sizes[i];
            
            delete [] Ptrs[i];
        }
        
        memcpy(AssembledData,&nebid,sizeof(i32));
        AssembledData += sizeof(i32);
        
        if((AssembledData - TotalSize) != *OutDataPtr)
        {
            ERROR("Something went wrong in the ConfigNode serialization process. Bytes written != Total size.\n");
        }
        
        return TotalSize;
    }
    size_t ConfigNode::Deserialize(Byte *InData)
    {
        intptr_t StartOffset = (intptr_t)InData;
        
        i32 bID        = (*(i32*)InData); InData += sizeof(i32 );                       //Node block ID
        if(bID != NODE_BEGIN_BLOCK_ID) return 0;
        i32 nLen       = (*(i32*)InData); InData += sizeof(i32 );                       //Node name length
        string Name    = string(InData,nLen); InData += sizeof(Byte) * m_Name.length(); //Node name
        bool HasValue  = (*(i32*)InData); InData += sizeof(bool);                       //Node contains value (bool)
        i32 ChildCount = (*(i32*)InData); InData += sizeof(i32 );                       //Child count
        
        if(Name != m_Name)
        {
            ERROR("Could not load config node, name mismatch. (%s != %s)\n",Name.c_str(),m_Name.c_str());
            return 0;
        }
        if(ChildCount != m_Children.size())
        {
            ERROR("Could not load config node, invalid number of children. (%d != %lu)\n",ChildCount,m_Children.size());
            return 0;
        }
        
        if(HasValue) InData += m_Value->Deserialize(InData);
        
        for(i32 i = 0;i < ChildCount;i++) InData += m_Children[i]->Deserialize(InData);
        return ((intptr_t)InData) - StartOffset;
    }
    
    Configuration::Configuration() : m_RootNode(new ConfigNode("Root"))
    {
    }
    Configuration::Configuration(const Configuration& c) : m_RootNode(new ConfigNode(*c.m_RootNode))
    {
    }
    Configuration::~Configuration()
    {
    }
    void Configuration::SetRootName(const string &Name)
    {
        m_RootNode->SetName(Name);
    }
    ConfigNode* Configuration::AddNode(const string &Name)
    {
        return m_RootNode->AddNode(Name);
    }
    size_t Configuration::Serialize(Byte** OutDataPtr)
    {
        Byte* ptr = 0;
        size_t NodeSz = m_RootNode->Serialize(&ptr);
        if(!ptr)
        {
            ERROR("Could not serialize root node.\n");
            return 0;
        }
        
        size_t TotalSize = 0;
        TotalSize += sizeof(Byte) * 4;                  //"cfg\0"
        TotalSize += sizeof(i32 );                      //CONFIG_MANAGER_VERSION_MAJOR
        TotalSize += sizeof(i32 );                      //CONFIG_MANAGER_VERSION_MINOR
        TotalSize += NodeSz;                            //Configuration contents
        TotalSize += sizeof(i32 );                      //File end ID
        
        char Header[4] = CONFIG_HEADER;
        i32 vMaj = CONFIG_MANAGER_VERSION_MAJOR;
        i32 vMin = CONFIG_MANAGER_VERSION_MINOR;
        i32 cEnd = CONFIG_FILE_END;
        
        Byte* AssembledData = (*OutDataPtr) = new Byte[TotalSize];
        memcpy(AssembledData,Header,sizeof(Byte) * 4); AssembledData += sizeof(Byte) * 4;
        memcpy(AssembledData,&vMaj ,sizeof(i32 )    ); AssembledData += sizeof(i32 )    ;
        memcpy(AssembledData,&vMin ,sizeof(i32 )    ); AssembledData += sizeof(i32 )    ;
        memcpy(AssembledData,ptr   ,NodeSz          ); AssembledData += NodeSz          ;
        memcpy(AssembledData,&cEnd ,sizeof(i32 )    ); AssembledData += sizeof(i32 )    ;
        
        if((AssembledData - TotalSize) != *OutDataPtr)
        {
            ERROR("Something went wrong in the Configuration serialization process. Bytes written != Total size.\n");
        }
        
        if(ptr) delete [] ptr;
        
        return TotalSize;
    }
    
    void Configuration::Deserialize(Byte* InData)
    {
        char Header[4] = CONFIG_HEADER;
        
        //Header
        if(InData[0] != Header[0]
        || InData[1] != Header[1]
        || InData[2] != Header[2]
        || InData[3] != Header[3])
        {
            ERROR("Could not deserialize data into configuration object. Invalid data header.\n");
            return;
        }
        InData += 4;
        
        //Version
        i32 vMajf = (*(i32*)InData); InData += sizeof(i32);
        i32 vMinf = (*(i32*)InData); InData += sizeof(i32);
        
        if(vMajf != CONFIG_MANAGER_VERSION_MAJOR || vMinf != CONFIG_MANAGER_VERSION_MINOR)
        {
            bool IsCompatible = false;
            //Decide if compatible
            
            if(IsCompatible)
            {
                //Convert
            }
            else ERROR("Incompatible configuration versions. (current: %d.%d, from file: %d,%d)\n",CONFIG_MANAGER_VERSION_MAJOR,
                                                                                                   CONFIG_MANAGER_VERSION_MINOR,
                                                                                                   vMajf,vMinf);
        }
        
        if(m_RootNode) delete m_RootNode;
        m_RootNode = new ConfigNode("Root");
        InData += m_RootNode->Deserialize(InData);
        
        //End
        i32 cEndf = (*(i32*)InData);
        if(cEndf != CONFIG_FILE_END)
        {
            ERROR("Warning: End of deserialize procedure reached before CONFIG_FILE_END.\n");
        }
    }
    
};