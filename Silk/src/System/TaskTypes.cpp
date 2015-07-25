#include <System/TaskTypes.h>

using namespace std;
namespace Silk
{
    string GetTaskTypeName(TASK_TYPE Type)
    {
        static string TaskNames[TT_COUNT] =
        {
            "Idle",
            "Culling (Brute Force)",
        };
        return TaskNames[Type];
    }
};