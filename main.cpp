#include "command.h"

int main(int argc, char *argv[]){
     system("chcp 65001");
     std::string default_command = "Pong";
     if (argc > 1) {
         default_command = argv[1];
     }
     std::cout << "User Commands: " << default_command << std::endl;

     while(true) {
         if (is_in_command(default_command))
            get_commands_set()->at(default_command)();
         else if (default_command == "exit") {
             std::cout << "Exiting..." << std::endl;
             return 0;
         }
         else {
             std::cout << "error commands!" << std::endl;
             print_commands();
         }
         std::cout << "Commands: " << std::endl;
         std::cin >> default_command;
     }
     return 0;
}