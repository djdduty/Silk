#include <Utilities/Utilities.h>

namespace Silk
{
    #define MAX_FORMAT_LENGTH 256
    string FormatString(const char* Text,...)
    {
        va_list Args;
        va_start(Args,Text);
        size_t len = strlen(Text) + MAX_FORMAT_LENGTH;
        char* fmt = new char[len];
        vsnprintf(fmt,len,Text,Args);
        va_end(Args);
        string r = fmt;
        delete [] fmt;
        return r;
    }
    i32 ParseFirstInt(const string& Text)
    {
        string n;
        bool t = false;
        for(i32 i = 0;i < Text.size();i++)
        {
            if(isdigit(Text[i]) || Text[i] == '-')
            {
                if(!t) t = true;
                n += Text[i];
            }
            else if(t) break;
        }
        
        return atoi(n.c_str());
    }
    i32 ParseInts(const string& Text,vector<i32>& OutInts)
    {
        string n;
        bool t = false;
        for(i32 i = 0;i < Text.size();i++)
        {
            if(isdigit(Text[i]) || Text[i] == '-')
            {
                if(!t) t = true;
                n += Text[i];
            }
            else if(t)
            {
                OutInts.push_back(atoi(n.c_str()));
                n.clear();
            }
        }
        return OutInts.size();
    }
    i32 ParseFirstIntAfter(const string& Text,const string& AfterThis)
    {
        size_t p = Text.find(AfterThis);
        if(p == Text.npos) return INT32_MAX;
        return ParseFirstInt(Text.substr(p,Text.length() - 1));
    }
    
    Scalar ParseFirstScalar(const string& Text)
    {
        string n;
        bool t = false;
        for(i32 i = 0;i < Text.size();i++)
        {
            if(isdigit(Text[i]) || Text[i] == '.' || Text[i] == '-')
            {
                if(!t) t = true;
                n += Text[i];
            }
            else if(t) break;
        }
        
        return atof(n.c_str());
    }
    i32 ParseScalars(const string& Text,vector<Scalar>& OutScalars)
    {
        string n;
        bool t = false;
        for(i32 i = 0;i < Text.size();i++)
        {
            if(isdigit(Text[i]) || Text[i] == '-')
            {
                if(!t) t = true;
                n += Text[i];
            }
            else if(t)
            {
                OutScalars.push_back(atof(n.c_str()));
                n.clear();
            }
        }
        return OutScalars.size();
    }
};
