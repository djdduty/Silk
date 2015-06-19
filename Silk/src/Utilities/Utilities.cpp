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
};
