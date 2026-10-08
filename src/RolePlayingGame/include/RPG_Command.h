#pragma once

#include "RPG_Dynamic.h"
#include <list>

namespace RoleGame {
    class RPG_Engine;
    class cQuest;

    class cCommand {
    public:
        cCommand() {
        }

        virtual ~cCommand() {
        }

        bool bStarted = false;
        bool bCompleted = false;

        virtual void Start() {
        }

        virtual void Update(float fElapsedTime) {
        };

        static RPG_Engine *g_engine;
    };


    class cScriptProcessor {
    public:
        cScriptProcessor();

    public:
        void AddCommand(cCommand *cmd);

        void ProcessCommands(float fElapsedTime);

        void CompleteCommand();

    public:
        bool bUserControlEnabled;

    private:
        std::list<cCommand *> m_listCommands;
    };


    class cCommand_MoveTo : public cCommand {
    public:
        cCommand_MoveTo(cDynamic *object, float x, float y, float duration = 0.0f);

        void Start() override;

        void Update(float fElapsedTime) override;

    private:
        cDynamic *m_pObject;
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


    class cCommand_ChangeMap : public cCommand {
    public:
        cCommand_ChangeMap(std::string mapName, float mapPosX, float mapPosY);

        void Start() override;

    private:
        std::string m_sMapName;
        float m_fMapPosX;
        float m_fMapPosY;
    };

    class cCommand_AddQuest : public cCommand {
    public:
        cCommand_AddQuest(cQuest *quest);

        void Start() override;

    private:
        cQuest *m_quest;
    };
}