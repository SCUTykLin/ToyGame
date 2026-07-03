//
// Created by 15813 on 2026/6/29.
//

#ifndef TOYPROJECTS_TOKEN_H
#define TOYPROJECTS_TOKEN_H

typedef enum {
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
}TokenTypeSeparator; // 分隔符

typedef enum{
    EXIT
}TokenTypeKeyword; // 关键字

typedef enum {
    INT,
}TokenTypeLiteral; // 字面量

typedef struct {

}TokenKeyword;

typedef struct{

}TokenLiteral;

#endif //TOYPROJECTS_TOKEN_H
