/*
 *  Common.cpp
 *  Silk
 *
 *  Created by Michael DeCicco on 10/6/15.
 *
 */

#include <UIElements/Common.h>

namespace TestClient
{
    UIText* AddText(UIManager* Mgr,Scalar Sz,const Vec2& Pos,const string& Text,UIPanel* Panel)
    {
        UIText* Tex = new UIText();
        Tex->SetFont(Mgr->GetFont());
        Tex->SetText(Text);
        Tex->SetTextSize(Sz);
        Tex->SetPosition(Vec3(Pos,0.0f));
        Panel->AddChild(Tex);
        return Tex;
    }
};
