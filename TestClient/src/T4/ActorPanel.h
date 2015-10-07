/*
 *  ActorPanel.h
 *  Silk
 *
 *  Created by Michael DeCicco on 10/4/15.
 *
 */

#pragma once
#include <Silk.h>
#include <T4/Turok4.h>
#include <InputManager.h>
#include <UIElements/TabPanel.h>
#include <UIElements/TablePanel.h>

using namespace TestClient;
using namespace Turok4;
using namespace Silk;

class ActorPanel : public UIPanel
{
    public:
        ActorPanel(UIManager* Mgr,InputManager* Input);
        ~ActorPanel();
    
        void Toggle();

        virtual void Update(Scalar dt);
        void SetActor(Actor* a);
    
    protected:
        TabPanel  * m_TabPanel ;
        TablePanel* m_VarTable ;
    
        UIText  * m_Heading    ;
        UIText  * m_ActorFile  ;
        UIText  * m_ActorName  ;
        UIText  * m_ActorID    ;
    
        Scalar    m_Translation;
        Scalar    m_Velocity   ;
};
