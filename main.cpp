#include "main.h"

int main(int argc, char *argv[]){
    system("chcp 65001");

    std::string default_command = "Pong";
    if (argc > 1) {
        default_command = argv[1];
    }
    std::cout << "User Commands: " << default_command << std::endl;

    get_commands_set()->at(default_command)();

    return 0;
}