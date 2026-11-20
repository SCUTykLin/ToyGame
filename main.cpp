#include "command.h"

#define FLTK_WIDTH 800
#define FLTK_HEIGHT 600
#define FLTK_BUTTON_WIDTH 120
#define FLTK_BUTTON_HEIGHT 50

#include <windows.h>
#include <clocale>
#include "SDL2/SDL.h"

int main(int argc, char *argv[]){
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    FLTK_Window::Set_FLTK_Style("oxy");
    FLTK_Window::init_window(FLTK_BUTTON_WIDTH, FLTK_BUTTON_HEIGHT, FLTK_WIDTH, FLTK_HEIGHT);
    return FLTK_Window::run();
}