#ifndef TOYPROJECTS_MAIN_H
#define TOYPROJECTS_MAIN_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include "src/PongGame/pong_game.h"
#include "src/BouncyBall/bouncy_ball.h"
#include "src/FluidSimulation/App/FluidApplication.h"
#include "src/Compiler/app.h"
#include "src/Roguelike/App/app.h"

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
    void run_command_Impl() {
        Pong::startPongGame();
    }
};

class Bouncy_Command : public Base_Command<Bouncy_Command> {
public:
    void run_command_Impl() {
        bouncy::start_bouncy();
    }
};

class Fluid_Command : public Base_Command<Fluid_Command> {
public:
    void run_command_Impl() {
        fluid::FluidApplication app;
        app.run();
    }
};

class Compiler : public Base_Command<Compiler> {
public:
    void run_command_Impl() {
        compiler::CompilerApp compilerApp;
        compilerApp.run_compile("test.unn");
    }
};

class Roguelike : public Base_Command<Roguelike> {
public:
    void run_command_Impl() {
        roguelike::App{}.run();
    }
};

template <typename T>
void add_command(T t, std::string&& name) {
    auto command = std::make_shared<T>();
    commands[name] = [command]()->void{command->run_command();};
}

void create_commands_set() {
    std::call_once(flag, []() {
        add_command(Pong_Command{}, "Pong");
        add_command(Bouncy_Command{}, "Bouncy");
        add_command(Fluid_Command{}, "Fluid");
        add_command(Compiler{}, "Compiler");
        add_command(Roguelike{}, "Roguelike");
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