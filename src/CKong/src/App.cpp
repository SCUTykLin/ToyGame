//
// Created by 15813 on 2026/7/21.
//

#include "../include/App.h"
#include "../include/str.h"

namespace CKong{
    void App::run() {
        Helper::cstring_t* s = CKong::Helper::string_clone("Hello world!");
        auto str_right = CKong::Helper::string_right(s, 5);
        CKong::Helper::string_print(str_right, stdout);
        Helper::string_free(s);
    }
}