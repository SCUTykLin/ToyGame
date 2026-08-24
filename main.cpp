#include "command.h"

#define FLTK_WIDTH 800
#define FLTK_HEIGHT 600
#define FLTK_BUTTON_WIDTH 120
#define FLTK_BUTTON_HEIGHT 50

int main(int argc, char *argv[]){
    system("chcp 65001");
    FLTK_Window::Set_FLTK_Style("oxy");
    FLTK_Window::init_window(FLTK_BUTTON_WIDTH, FLTK_BUTTON_HEIGHT, FLTK_WIDTH, FLTK_HEIGHT);
    return FLTK_Window::run();
}