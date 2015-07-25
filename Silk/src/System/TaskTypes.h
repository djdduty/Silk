#pragma once
#include <string>
using namespace std;
namespace Silk
{
    enum TASK_TYPE
    {
        TT_THREAD_IDLE_TIME,
        TT_BRUTE_FORCE_CULL_TASK,
        TT_COUNT,
    };

    string GetTaskTypeName(TASK_TYPE Type);
};