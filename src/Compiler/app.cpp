//
// Created by YangKai.Lin on 2026/7/11.
//

#include "app.h"
#include <filesystem>

namespace compiler {

    void CompilerApp::run_compile(std::string fileName) {
        FILE* file = openFile(fileName);
        lexerPtr = new Lexer(file);
    }

    FILE * CompilerApp::openFile(std::string fileName) {
        FILE *file = fopen(fileName.c_str(), "r");
        if (!file) {
            std::cerr << "Error opening file :" << fileName << std::endl;
            std::cout << "Current Work Dir: " << std::filesystem::current_path() << '\n';
            exit(1);
        }
        return file;
    }

}