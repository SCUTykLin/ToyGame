//
// Created by 15813 on 2026/8/13.
//

#include "../include/types.h"
#include <cstring>

namespace Compiler::Types{
    int typename_to_int(const char* name){
        int t = 0;
        size_t len = strlen(name);

        for (unsigned int i = 0; i < len; ++i){
            t += name[i];
        }
        return t;
    }
}
