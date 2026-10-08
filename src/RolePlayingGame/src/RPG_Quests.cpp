#include "RPG_Items.h"
#include "RPG_Engine.h"

#define X(n) g_script->AddCommand(new cCommand_ ## n)

namespace RoleGame {
    cScriptProcessor* cQuest::g_script = nullptr;
    RPG_Engine* cQuest::g_engine = nullptr;
    cQuest::cQuest() {}

    bool cQuest::PopulateDynamics(std::vector<cDynamic *> &vecDyns, std::string sMap) {
        return true;
    }

    bool cQuest::OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
        return true;
    }


    bool cQuest_MainQuest::PopulateDynamics(std::vector<cDynamic *> &vecDyns, std::string sMap) {
        if (sMap == "coder town") {
            cDynamic_Creature *c1 = new cDynamic_Creature("sarah", RPG_Assets::get().GetSprite("purple"));
            c1->px = 6.0f;
            c1->py = 4.0f;
            c1->bFriendly = true;
            vecDyns.push_back(c1);
        }

        if (sMap == "home") {
            cDynamic_Creature *c1 = new cDynamic_Creature("bob", RPG_Assets::get().GetSprite("skelly"));
            c1->px = 12.0f;
            c1->py = 4.0f;
            vecDyns.push_back(c1);
        }


        return true;
    }

    bool cQuest_MainQuest::OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
        if (target->sName == "sarah") {
            //X(ShowDialog({ "[Sarah]", "You have no additional", "quests!" }));

            if (g_engine->HasItem(RPG_Assets::get().GetItem("Health Boost"))) {
                X(ShowDialog({ "[Sarah]", "Woooooow! You have a health", "boost!" }));
            } else {
                X(ShowDialog({ "[Sarah]", "Boooooo! You dont have a health", "boost!" }));
            }
        }


        if (target->sName == "bob") {
            X(ShowDialog({ "[Bob]", "I need you to do", "something for me!" }));
            X(ShowDialog({ "[Bob]", "Predictably, there are", "rats in my basement!"}));
            X(AddQuest(new cQuest_BobsQuest()));
        }

        return false;
    }


    bool cQuest_BobsQuest::PopulateDynamics(std::vector<cDynamic *> &vecDyns, std::string sMap) {
        return true;
    }

    bool cQuest_BobsQuest::OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
        if (target->sName == "sarah") {
            X(ShowDialog({ "[Sarah]", "You are doing Bob's", "quest!" }));
            return true;
        }

        return false;
    }
}
