#ifndef TOYPROJECTS_MAIN_H
#define TOYPROJECTS_MAIN_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include <iostream>
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Button.h>

extern  std::unordered_map<std::string, std::function<void()>> commands;
extern  std::once_flag flag;

template <typename Derived>
class Base_Command {
public:
    void run_command() {
        static_cast<Derived*>(this)->run_command_Impl();
    }
};

class Pong_Command : public Base_Command<Pong_Command> {
public:
    void run_command_Impl();
};

class Bouncy_Command : public Base_Command<Bouncy_Command> {
public:
    void run_command_Impl();
};

class Fluid_Command : public Base_Command<Fluid_Command> {
public:
    void run_command_Impl();
};

class Compiler_Command : public Base_Command<Compiler_Command> {
public:
    void run_command_Impl();
};

class Roguelike_Command : public Base_Command<Roguelike_Command> {
public:
    void run_command_Impl();
};

class SDL_Engine_Command : public Base_Command<SDL_Engine_Command> {
public:
    void run_command_Impl();
};


class SDLRayTracer_Command : public Base_Command<SDLRayTracer_Command> {
    public:
    void run_command_Impl();
};

class ImGUIDesign_001_Command : public Base_Command<ImGUIDesign_001_Command> {
public:
    void run_command_Impl();
};

class Tetris_Command : public Base_Command<Tetris_Command> {
public:
    void run_command_Impl();
};

class ConFPS_Command : public Base_Command<ConFPS_Command> {
public:
    void run_command_Impl();
};

class CloxInterpreter_Command : public Base_Command<CloxInterpreter_Command> {
public:
    void run_command_Impl();
};

template <typename T>
void add_command(T t, std::string&& name) {
    auto command = std::make_shared<T>();
    commands[name] = [command]()->void{command->run_command();};
}

void create_commands_set();

std::unordered_map<std::string, std::function<void()>>* get_commands_set();

void print_commands();

bool is_in_command(std::string& command_str);

std::vector<std::string> get_commands_list();

class FLTK_Window {
public:
    static void init_window(int w, int h, int Width, int Height) {
        if (win != nullptr) {
            return;
        }

        win = new Fl_Window(Width, Height, "Commands Window");

        win->begin();

        add_command_button(w, h);

        win->end();
        win->show();
    }

    static void add_button(int x, int y, int w, int h, const char* name) {
        if (win == nullptr) {
            return;
        }
        auto* callback_name = new std::string(name);
        auto* button = new Fl_Button(x, y, w, h, callback_name->c_str());
        button->callback([](Fl_Widget*, void* data) {
            const auto& name = *static_cast<std::string*>(data);
            get_commands_set()->at(name)();
        }, callback_name);
    }

    static void add_command_button(int w, int h) {
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

    static int run() {
        return Fl::run();
    }

private:
    inline static Fl_Window* win = nullptr;
};

#endif //TOYPROJECTS_MAIN_H