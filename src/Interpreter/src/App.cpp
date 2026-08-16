//
// Created by YangKai.Lin on 2026/8/5.
//

#include "App.h"
#include "vm.h"
#include "file.h"
#include <format>
#include <iostream>

namespace CLOX {

    static void repl() {
        char line[1024];
        for (;;) {
            printf("> ");

            if (!fgets(line, sizeof(line), stdin)) {
                printf("\n");
                break;
            }

            Vm::interpret(line);
        }
    }

    void App::run() {
        Vm::initVM();
        const char* filePath = "test.txt";
        std::cout << std::format("Usage: clox [path]: {}", filePath) << std::endl;
        File::runFile(filePath);

        Vm::freeVM();
    }
}