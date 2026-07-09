//
// Created by YangKai.Lin on 2026/7/7.
//

#ifndef TOYPROJECTS_MAIN_H
#define TOYPROJECTS_MAIN_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include "src/PongGame/pong_game.h"
#include "src/BouncyBall/bouncy_ball.h"
#include "src/FluidSimulation/fluid.h"

static std::unordered_map<std::string, std::function<void()>> commands;
static std::once_flag flag;

template <typename Derived>
class Base_Command {
public:
    void run_command() {
        static_cast<Derived*>(this)->run_command_Impl();
    }
};

class Pong_Command : public Base_Command<Pong_Command> {
public:
    Pong_Command() {}
    void run_command_Impl() {
        Pong::startPongGame();
    }
};

class Bouncy_Command : public Base_Command<Bouncy_Command> {
public:
    Bouncy_Command() {}
    void run_command_Impl() {
        bouncy::start_bouncy();
    }
};

class Fluid_Command : public Base_Command<Fluid_Command> {
public:
    Fluid_Command() {}
    void run_command_Impl() {
        fluid::start_fluid();
    }
};

void create_commands_set() {
    std::call_once(flag, []() {
        auto pong_command = std::make_shared<Pong_Command>();
        commands["Pong"] = [pong_command]()->void{pong_command->run_command();};
        auto bouncy_command = std::make_shared<Bouncy_Command>();
        commands["Bouncy"] = [bouncy_command]()->void{bouncy_command->run_command();};
        auto fluid_command = std::make_shared<Fluid_Command>();
        commands["Fluid"] = [fluid_command]()->void{fluid_command->run_command();};
    });
}

std::unordered_map<std::string, std::function<void()>>* get_commands_set() {
    create_commands_set();
    std::cout << "Commands List: [ ";
    for (const auto& pair : commands) {
        std::cout << pair.first << " ";  // key
    }
    std::cout << "]" << std::endl;

    return &commands;
}

#endif //TOYPROJECTS_MAIN_H