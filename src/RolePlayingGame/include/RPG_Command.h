//
// Created by YangKai.Lin on 2026/10/3.
//

#ifndef TOYPROJECTS_RoleGame_RPG_COMMAND_H
#define TOYPROJECTS_RoleGame_RPG_COMMAND_H
#include <list>
#include "RPG_Dynamic.h"

class RPG_Engine;

namespace RoleGame {
    class cCommand {
    public:
        cCommand() {}
        virtual ~cCommand() {}

        bool bCompleted = false;
        bool bStarted = false;

        virtual void Start(){}
        virtual void Update(float fElapsedTime){}

        static RPG_Engine* g_engine;
    };

    class cScriptProcessor {
    public:
        cScriptProcessor();
    public:
        void AddCommand(cCommand* cmd);
        void ProcessCommands(float fElapsedTime);
        void CompleteCommand();
    public:
        bool bUserControlEnabled;
    private:
        std::list<cCommand*> m_listCommands;
    };

    class cCommand_MoveTo : public cCommand {
    public:
        cCommand_MoveTo(cDynamic* object, float x, float y, float duration = 0.0f);
        void Start() override;
        void Update(float fElapsedTime) override;
    private:
        cDynamic* m_pObject;
        float m_fStartPosX;
        float m_fStartPosY;
        float m_fTargetPosX;
        float m_fTargetPosY;
        float m_fDuration;
        float m_fTimeSoFar;
    };

    class cCommand_ShowDialog : public cCommand {
    public:
        cCommand_ShowDialog(std::vector<std::string> line);
        void Start() override;
    private:
        std::vector<std::string> vecLines;
    };
}

#endif //TOYPROJECTS_RoleGame_RPG_COMMAND_H
