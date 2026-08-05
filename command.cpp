//
// Created by YangKai.Lin on 2026/8/5.
//

#include <sstream>
#include "command.h"
#include "src/PongGame/pong_game.h"
#include "src/BouncyBall/bouncy_ball.h"
#include "src/FluidSimulation/App/FluidApplication.h"
#include "src/Compiler/include/app.h"
#include "src/Roguelike/App/app.h"
#include "src/Madsycode/App/app.h"
#include "src/SDL2RayTracer/include/App.h"
#include "src/UI/ImGUIDesign/ImGUI_001/include/App.h"
#include "src/Tetris/include/App.h"
#include "src/ConFPS/include/App.h"
#include "src/Interpreter/include/App.h"

std::unordered_map<std::string, std::function<void()>> commands;
std::once_flag flag;

void Pong_Command::run_command_Impl() {
    Pong::startPongGame();
}

void Bouncy_Command::run_command_Impl() {
    bouncy::start_bouncy();
}

void Fluid_Command::run_command_Impl() {
    fluid::FluidApplication{}.run();
}

void Compiler_Command::run_command_Impl() {
    Compiler::App{}.run_compile("src/Compiler/examples/main.tac");
}

void Roguelike_Command::run_command_Impl() {
    roguelike::App{}.run();
}

void SDL_Engine_Command::run_command_Impl() {
    Madsycode::App{}.run();
}

void SDLRayTracer_Command::run_command_Impl() {
    RayTracing::App{}.run();
}

void ImGUIDesign_001_Command::run_command_Impl() {
    UI::ImGUI_001::App{}.run();
}

void Tetris_Command::run_command_Impl() {
    Tetris::App{}.run();
}

void ConFPS_Command::run_command_Impl() {
    ConFPS::App{}.run();
}

void CloxInterpreter_Command::run_command_Impl() {
    CLOX::App{}.run();
}

void create_commands_set() {
    std::call_once(flag, []() {
        add_command(Pong_Command{}, "Pong");
        add_command(Bouncy_Command{}, "Bouncy");
        add_command(Fluid_Command{}, "Fluid");
        add_command(Compiler_Command{}, "Compiler");
        add_command(Roguelike_Command{}, "Roguelike");
        add_command(SDL_Engine_Command{}, "2DEngine");
        add_command(SDLRayTracer_Command{}, "SDLRayTracer");
        add_command(ImGUIDesign_001_Command{}, "ImGUIDesign_001");
        add_command(Tetris_Command{}, "Tetris");
        add_command(ConFPS_Command{}, "ConFPS");
    });
}

std::unordered_map<std::string, std::function<void()>>* get_commands_set() {
    create_commands_set();
    std::cout << "Commands List: [";
    for (const auto& pair : commands) {
        std::cout << pair.first << ", ";  // key
    }
    std::cout << "]" << std::endl;

    return &commands;
}

void print_commands() {
    std::stringstream ss;
    for (const auto& pair : commands) {
        ss << pair.first << ", ";
    }
    std::string_view v = ss.view();
    std::cout << "Commands List: [" << v << "]" << std::endl;
}

std::vector<std::string> get_commands_list() {
    create_commands_set();
    std::vector<std::string> command_lists;
    for (const auto& pair : commands) {
        command_lists.emplace_back(pair.first);
    }
    return command_lists;
}

bool is_in_command(std::string& command_str){
    create_commands_set();
    for (const auto& pair : commands) {
        if (pair.first == command_str) {
            return true;
        }
    }
    return false;
}