//
// Created by 15813 on 2026/8/21.
//

#include "Token.h"
#include <sstream>
#include <utility>

namespace CLOX_V1::Token{
    Token::Token(TokenType type, std::string lexeme, std::any literal, int line) : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line){}

    std::string Token::toString() {
        std::stringstream ss;
        if (this->type == NUMBER) {
            ss << "[Type]: " << type2str(type) << " [Lexeme]: (" << lexeme << ") [Literal]: (" << std::any_cast<double>(literal) << ")";
        }else{
            ss << "[Type]: " << type2str(type) << " [Lexeme]: (" << lexeme << ") [Literal]: (" << std::any_cast<std::string>(literal) <<")";
        }
        return ss.str();
    }

    std::string Token::type2str(TokenType_t type){
        std::string printStr = "EOF";
        switch (type) {
            case LEFT_PAREN:
                printStr = "LEFT_PAREN";
                break;
            case RIGHT_PAREN:
                printStr = "RIGHT_PAREN";
                break;
            case LEFT_BRACE:
                printStr = "LEFT_BRACE";
                break;
            case RIGHT_BRACE:
                printStr = "RIGHT_BRACE";
                break;
            case COMMA:
                printStr = "COMMA";
                break;
            case DOT:
                printStr = "DOT";
                break;
            case MINUS:
                printStr = "MINUS";
                break;
            case PLUS:
                printStr = "PLUS";
                break;
            case SEMICOLON:
                printStr = "SEMICOLON";
                break;
            case SLASH:
                printStr = "SLASH";
                break;
            case STAR:
                printStr = "STAR";
                break;
            case BANG:
                printStr = "BANG";
                break;
            case BANG_EQUAL:
                printStr = "BANG_EQUAL";
                break;
            case EQUAL:
                printStr = "EQUAL";
                break;
            case EQUAL_EQUAL:
                printStr = "EQUAL_EQUAL";
                break;
            case GREATER:
                printStr = "GREATER";
                break;
            case GREATER_EQUAL:
                printStr = "GREATER_EQUAL";
                break;
            case LESS:
                printStr = "LESS";
                break;
            case LESS_EQUAL:
                printStr = "LESS_EQUAL";
                break;
            case IDENTIFIER:
                printStr = "IDENTIFIER";
                break;
            case STRING:
                printStr = "STRING";
                break;
            case NUMBER:
                printStr = "NUMBER";
                break;
            case AND:
                printStr = "AND";
                break;
            case CLASS:
                printStr = "CLASS";
                break;
            case ELSE:
                printStr = "ELSE";
                break;
            case FALSE:
                printStr = "FALSE";
                break;
            case FUN:
                printStr = "FUN";
                break;
            case FOR:
                printStr = "FOR";
                break;
            case IF:
                printStr = "IF";
                break;
            case NIL:
                printStr = "NIL";
                break;
            case OR:
                printStr = "OR";
                break;
            case PRINT:
                printStr = "PRINT";
                break;
            case RETURN:
                printStr = "RETURN";
                break;
            case SUPER:
                printStr = "SUPER";
                break;
            case THIS:
                printStr = "THIS";
                break;
            case TRUE:
                printStr = "TRUE";
                break;
            case VAR:
                printStr = "VAR";
                break;
            case WHILE:
                printStr = "WHILE";
                break;
            case EOF_Type:
                printStr = "EOF";
                break;
            case COMMENTS:
                printStr = "COMMENTS";
                break;
            default:
                printStr = "UnKnow";
                break;
        }
        return printStr;
    }

}