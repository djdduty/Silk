#pragma once
#include <Utilities/Font.h>
#include <Utilities/SimplexNoise.h>
#include <Utilities/PerlinNoise.h>
#include <Utilities/Timer.h>
#include <Utilities/SampleBuffer.h>
#include <Utilities/ConfigurationManager.h>

#include <stdarg.h>

#include <string>
using namespace std;

namespace Silk
{
    string FormatString(const char* Text,...);
    
    i32 ParseFirstInt(const string& Text);
    i32 ParseInts(const string& Text,vector<i32>& OutInts);
    
    i32 ParseFirstIntAfter(const string& Text,const string& AfterThis);
    
    Scalar ParseFirstScalar(const string& Text);
    i32 ParseScalars(const string& Text,vector<Scalar>& OutScalars);
};

