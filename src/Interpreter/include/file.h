//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_FILE_H
#define TOYPROJECTS_CLOX_FILE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "file.h"
#include <iostream>

namespace CLOX::File{

    static char* readFile(const char* path) {
        FILE* file = fopen(path, "rb");
        if (file == nullptr) {
            fprintf(stderr, "Could not open file \"%s\".\n", path);
            exit(74);
        }

        fseek(file, 0L, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);

        char* buffer = (char*)malloc(fileSize + 1);
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

    static void runFile(const char* path) {
        char* source = readFile(path);

        std::cout << "Decode String: [" << source << "] " << std::endl;

        Vm::InterpretResult result = Vm::interpret(source);
        free(source);

        if (result == Vm::INTERPRET_COMPILE_ERROR) exit(65);
        if (result == Vm::INTERPRET_RUNTIME_ERROR) exit(70);
    }
}

#endif //TOYPROJECTS_CLOX_FILE_H
