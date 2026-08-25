//
// Created by YangKai.Lin on 2026/8/5.
//

#include <sstream>
#include "command.h"
#include "src/PongGame/pong_game.h"
#include "src/BouncyBall/bouncy_ball.h"
#include "src/FluidSimulation/App/FluidApplication.h"
#include "src/Roguelike/App/app.h"
#include "src/Madsycode/App/app.h"
#include "src/SDL2RayTracer/include/App.h"
#include "src/UI/ImGUIDesign/ImGUI_001/include/App.h"
#include "src/Tetris/include/App.h"
#include "src/ConFPS/include/App.h"
#include "src/Interpreter_v1/include/App.h"
#include "src/UpFPS/include/App.h"
#include "src/RacingGame/include/App.h"
#include "src/Frogger/include/App.h"
#include "src/TextEditor/include/App.h"

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

void UpFPS_Command::run_command_Impl() {
    UpFPS::App{}.run();
}

void CloxInterpreter_Command::run_command_Impl() {
    CLOX_V1::App{}.run();
}

void RacingGame_Command::run_command_Impl() {
    Racing::App{}.run();
}

void Frogger_Command::run_command_Impl() {
    Frogger::App{}.run();
}

void TextEditor_Command::run_command_Impl() {
    TextEditor::App{}.run();
}

void create_commands_set() {
    std::call_once(flag, []() {
        add_command(Pong_Command{}, "Pong");
        add_command(Bouncy_Command{}, "Bouncy");
        add_command(Fluid_Command{}, "Fluid");
        add_command(Roguelike_Command{}, "Roguelike");
        add_command(SDL_Engine_Command{}, "2DEngine");
        add_command(SDLRayTracer_Command{}, "SDLRayTracer");
        add_command(ImGUIDesign_001_Command{}, "ImGUIDesign_001");
        add_command(Tetris_Command{}, "Tetris");
        add_command(ConFPS_Command{}, "ConFPS");
        add_command(UpFPS_Command{}, "UpFPS");
        add_command(CloxInterpreter_Command{}, "CloxInterpreter");
        add_command(RacingGame_Command{}, "RacingGame");
        add_command(Frogger_Command{}, "Frogger");
        add_command(TextEditor_Command{}, "TextEditor");
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

Fl_Window* FLTK_Window::win = nullptr;
void FLTK_Window::Set_FLTK_Style(const char* style) {
    Fl::scheme(style);
    Fl::box_border_radius_max(8);

    Fl::background(30, 32, 36);
    Fl::background2(40, 43, 48);
    Fl::foreground(225, 228, 232);
}

void FLTK_Window::Set_Button_Style(Fl_Button* button) {
    button->box(FL_ROUNDED_BOX);

    button->color(fl_rgb_color(55, 59, 67));
    button->selection_color(fl_rgb_color(70, 125, 220));

    button->labelcolor(fl_rgb_color(235, 238, 242));
    button->labelfont(FL_HELVETICA);
    button->labelsize(14);

    button->clear_visible_focus();
}

 void FLTK_Window::init_window(int w, int h, int Width, int Height) {
    if (win != nullptr) {
        return;
    }

    win = new Fl_Window(Width, Height, "Commands Window");

    win->begin();

    add_command_button(w, h);

    win->end();
    win->show();
}

 void FLTK_Window::add_button(int x, int y, int w, int h, const char* name) {
    if (win == nullptr) {
        return;
    }
    auto* callback_name = new std::string(name);
    auto* button = new Fl_Button(x, y, w, h, callback_name->c_str());
    button->callback([](Fl_Widget*, void* data) {
        const auto& name = *static_cast<std::string*>(data);
        get_commands_set()->at(name)();
    }, callback_name);

    Set_Button_Style(button);
}

 void FLTK_Window::add_command_button(int w, int h) {
    auto commands = get_commands_list();

    int button_count = commands.size();
    int rows = 8;
    int cols = button_count / rows + 1;

    int col_space = (win->w() - cols * w) / (cols + 1);
    int row_space = (win->h() - rows * h) / (rows + 1);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            if (index < commands.size()) {
                add_button(j * w + (j + 1) * col_space, i * h + (i + 1) * row_space, w, h, commands.at(index).c_str());
            }
        }
    }
}

 int FLTK_Window::run() {
    return Fl::run();
}