//
// Created by YangKai.Lin on 2026/7/11.
//

#ifndef TOYPROJECTS_COMPILER_H
#define TOYPROJECTS_COMPILER_H
namespace compiler {
    typedef enum {
        SEMI,
        OPEN_PAREN,
        CLOSE_PAREN,
    }TypeSeperator;

    typedef enum {
        EXIT,
    }TypeKeyword;

    typedef enum {
        INT,
    }TypeLiteral;
    typedef struct TokenKeyword{
        TypeKeyword type;
    } TokenKeyword;

    typedef struct TokenSeparator{
        TypeSeperator type;
    } TokenSeparator;

    typedef struct TokenLiteral{
        TypeLiteral type;
        char* value;
    } TokenLiteral;
}

#endif //TOYPROJECTS_COMPILER_H