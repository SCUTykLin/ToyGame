//
// Created by 15813 on 2026/8/16.
//
#include "scanner.h"

namespace CLOX::Scan{

    typedef struct {
        const char* start;
        const char* current;
        int line;
    } Scanner;

    Scanner scanner;

    void initScanner(const char* source){
        scanner.start = source;
        scanner.current = source;
        scanner.line = 1;
    }

    Token scanToken() {
        scanner.start = scanner.current;
        if (isAtEnd()) return makeToken(TOKEN_EOF);
        return errorToken("Unexpected character.");
    }
}