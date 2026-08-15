//
// Created by YangKai.Lin on 2026/8/5.
//

#include "App.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

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
        // 替换部分开始
        if (argc == 1) {
            repl();
        } else if (argc == 2) {
            runFile(argv[1]);
        } else {
            fprintf(stderr, "Usage: clox [path]\n");
            exit(64);
        }

        Vm::freeVM();
        // 替换部分结束
        return 0;
    }
}