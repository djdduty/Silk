/*
 *  Common.h
 *  TestClient
 *
 *  Created by Michael DeCicco on 10/6/15.
 *
 */

#pragma once
#include <Silk.h>
using namespace Silk;

namespace TestClient
{
    UIText* AddText(UIManager* Mgr,Scalar Sz,const Vec2& Pos,const string& Text,UIPanel* Panel);
};

