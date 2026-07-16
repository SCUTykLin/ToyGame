//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_LOG_H
#define TOYPROJECTS_LOG_H

#include <iostream>
#include <string>
namespace RPG_Platformer {

    template <typename ... Args>
    concept is_stream = requires(Args ... args){
        ((std::cout << args), ...);
    };

    template <is_stream ... Args>
    void print(Args ... args) {
        std::cout << "[RPG PLATFORMER LOG]===> ";
        ((std::cout << args), ...);
        std::cout << " <===[RPG PLATFORMER LOG]" << std::endl;
    }
}

#endif //TOYPROJECTS_LOG_H