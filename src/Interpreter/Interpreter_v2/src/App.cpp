//
// Created by YangKai.Lin on 2026/11/1.
//

#include "chunk.h"
#include "debug.h"
#include "App.h"
#include "vm.h"
#include <cstdio>
#include <cstdlib>

namespace CLOX_V2 {
    static char *readFile(const char *path) {
        FILE *file = fopen(path, "rb");

        if (file == nullptr) {
            fprintf(stderr, "Could not open file \"%s\".\n", path);
            exit(74);
        }

        fseek(file, 0L, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);

        char *buffer = (char *) malloc(fileSize + 1);
        if (buffer == nullptr) {
            fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
            exit(74);
        }
        size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
        if (bytesRead < fileSize) {
            fprintf(stderr, "Could not read file \"%s\".\n", path);
            exit(74);
        }
        buffer[bytesRead] = '\0';

        fclose(file);
        return buffer;
    }

    static void runFile(const char *path) {
        char *source = readFile(path);
        VM::InterpretResult result = VM::interpret(source);
        free(source);

        if (result == VM::INTERPRET_COMPILE_ERROR) exit(65);
        if (result == VM::INTERPRET_RUNTIME_ERROR) exit(70);
    }

    static void repl() {
        char line[1024];
        for (;;) {
            printf("> ");

            if (!fgets(line, sizeof(line), stdin)) {
                printf("\n");
                break;
            }

            VM::interpret(line);
        }
    }

    void App::run(const char *fileName, bool fromFile) {
        VM::initVM();

        if (!fromFile) repl();
        else runFile(fileName);

        VM::freeVM();
    }
};
