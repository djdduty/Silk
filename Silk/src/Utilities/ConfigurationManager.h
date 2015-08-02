#pragma once
#include <System/SilkTypes.h>
#include <Math/Math.h>

#include <string>
#include <vector>
using namespace std;

#define CONFIG_MANAGER_VERSION_MAJOR 0
#define CONFIG_MANAGER_VERSION_MINOR 1

namespace Silk
{
    class ConfigValue
    {
        public:
            enum VALUE_TYPE
            {
                VT_BYTE,
                VT_UBYTE,
                VT_STRING,
                VT_BOOL,
                VT_I16,
                VT_U16,
                VT_I32,
                VT_U32,
                VT_I64,
                VT_U64,
                VT_F32,
                VT_F64,
                VT_VEC2,
                VT_VEC3,
                VT_VEC4,
                VT_MAT4,
                VT_COUNT,
            };
        
            ConfigValue(VALUE_TYPE Type,void* Ptr);
            ~ConfigValue();
        
            void Set(Byte          v);
            void Set(uByte         v);
            void Set(const string& v);
            void Set(bool          v);
            void Set(i16           v);
            void Set(u16           v);
            void Set(i32           v);
            void Set(u32           v);
            void Set(i64           v);
            void Set(u64           v);
            void Set(f32           v);
            void Set(f64           v);
            void Set(const Vec2&   v);
            void Set(const Vec3&   v);
            void Set(const Vec4&   v);
            void Set(const Mat4&   v);
        
            operator Byte  () const;
            operator uByte () const;
            operator string() const;
            operator bool  () const;
            operator i16   () const;
            operator u16   () const;
            operator i32   () const;
            operator u32   () const;
            operator i64   () const;
            operator u64   () const;
            operator f32   () const;
            operator f64   () const;
            operator Vec2  () const;
            operator Vec3  () const;
            operator Vec4  () const;
            operator Mat4  () const;
        
            VALUE_TYPE GetType() const { return m_Type; }
            string GetTypeName() const;
        
            bool HasChanged() const { return m_DidChange; }
            void ResetModificationFlag() { m_DidChange = false; }
        
            void DebugPrintValue();
            string GetDebugPrintString();
        
            size_t Serialize  (Byte** OutDataPtr);
            size_t Deserialize(Byte* InData);
        
        protected:
            void* m_TargetValue;
            size_t m_TypeSize;
            VALUE_TYPE m_Type;
            bool m_DidChange;
    };
    
    class ConfigNode
    {
        public:
            ConfigNode(const string& Name) : m_Name(Name), m_Value(0) { }
            ~ConfigNode();
        
            void Initialize(ConfigValue::VALUE_TYPE Type,void* Ptr);
            ConfigNode* AddNode(const string& Name);
        
            const vector<ConfigNode*>& GetChildren() const { return m_Children; }
            ConfigValue& GetValue() { return *m_Value; }
            void SetName(const string& Name) { m_Name = Name; }
            string GetName() const { return m_Name; }
        
            size_t Serialize  (Byte** OutDataPtr);
            size_t Deserialize(Byte* InData);
        
        protected:
            string m_Name;
            ConfigValue *m_Value;
            vector<ConfigNode*> m_Children;
    };
    
    class Configuration
    {
        public:
            Configuration();
            Configuration(const Configuration& c);
            ~Configuration();
        
            void SetRootName(const string& Name);
            ConfigNode* AddNode(const string& Name);
        
            size_t Serialize(Byte** OutDataPtr);
            void Deserialize(Byte* InData);
        
        protected:
            ConfigNode* m_RootNode;
    };
};