//
// Created by YangKai.Lin on 2026/7/11.
//

#include "../include/app.h"
#include "../include/tac.h"

namespace Compiler {
        void App::run_compile(const char* filename){
                tac::tac_complie_file(filename);
        }
}