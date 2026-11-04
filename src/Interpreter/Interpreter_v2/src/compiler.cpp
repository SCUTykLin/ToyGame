//
// Created by YangKai.Lin on 2026/11/3.
//
#include "compiler.h"

#include <cstdio>
#include <iterator>
#include <cstdlib>
#include "chunk.h"
#include "common.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

namespace CLOX_V2::Compiler {
    typedef struct {
        Scan::Token current;
        Scan::Token previous;
        bool hadError;
        bool panicMode;
    } Parser;

    typedef enum {
        PREC_NONE,
        PREC_ASSIGNMENT, // =
        PREC_OR, // or
        PREC_AND, // and
        PREC_EQUALITY, // == !=
        PREC_COMPARISON, // < > <= >=
        PREC_TERM, // + -
        PREC_FACTOR, // * /
        PREC_UNARY, // ! -
        PREC_CALL, // . ()
        PREC_PRIMARY
    } Precedence;

    typedef void (*ParseFn)();

    typedef struct {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
    } ParseRule;

    Parser parser;
    Chunk::Chunk *compilingChunk;

    static void expression();

    static ParseRule *getRule(Scan::TokenType type);

    static void parsePrecedence(Precedence precedence);

    static Chunk::Chunk *currentChunk() {
        return compilingChunk;
    }

    static void errorAt(Scan::Token *token, const char *message) {
        if (parser.panicMode) return;
        parser.panicMode = true;
        fprintf(stderr, "[line %d] Error", token->line);

        if (token->type == Scan::TOKEN_EOF) {
            fprintf(stderr, " at end");
        } else if (token->type == Scan::TOKEN_ERROR) {
            // Nothing.
        } else {
            fprintf(stderr, " at '%.*s'", token->length, token->start);
        }

        fprintf(stderr, ": %s\n", message);
        parser.hadError = true;
    }

    static void error(const char *message) {
        errorAt(&parser.previous, message);
    }

    static void errorAtCurrent(const char *message) {
        errorAt(&parser.current, message);
    }

    /**
     * Token: 从文本中扫描，获得Token
     * Parser: 从Token List中扫描获得Token
     */
    static void advance() {
        parser.previous = parser.current;

        for (;;) {
            parser.current = Scan::scanToken();
            if (parser.current.type != Scan::TOKEN_ERROR) break;
            errorAtCurrent(parser.current.start);
        }
    }

    static void consume(Scan::TokenType type, const char *message) {
        if (parser.current.type == type) {
            advance();
            return;
        }
        errorAtCurrent(message);
    }

    static void emitByte(uint8_t byte) {
        /**
         * currentChunk: 当前存储的指令块
         * byte: 当写要写入的指令
         * line: 代码第几行
         */
        Chunk::writeChunk(currentChunk(), byte, parser.previous.line);
    }

    static void emitBytes(uint8_t byte1, uint8_t byte2) {
        emitByte(byte1);
        emitByte(byte2);
    }

    static void emitReturn() {
        emitByte(Chunk::OP_RETURN);
    }

    static void endCompiler() {
        emitReturn();
#ifdef DEBUG_PRINT_CODE
        if (!parser.hadError) {
            Debug::disassembleChunk(currentChunk(), "code");
        }
#endif
    }

    // 从当前位置开始读一个算式
    static void expression() {
        parsePrecedence(PREC_ASSIGNMENT);
    }

    static uint8_t makeConstant(Const::Value value) {
        int constant = Chunk::addConstant(currentChunk(), value);
        if (constant > UINT8_MAX) {
            error("Too many constants in one chunk.");
            return 0;
        }
        return (uint8_t) constant;
    }

    static void emitConstant(Const::Value value) {
        emitBytes(Chunk::OP_CONSTANT, makeConstant(value));
    }

    static void number() {
        double value = strtod(parser.previous.start, nullptr);
        emitConstant(value);
    }

    static void grouping() {
        expression();
        consume(Scan::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    }

    static void unary() {
        Scan::TokenType operatorType = parser.previous.type;
        parsePrecedence(PREC_UNARY);
        switch (operatorType) {
            case Scan::TOKEN_MINUS: emitByte(Chunk::OP_NEGATE);
                break;
            default: return;
        }
    }

    static void binary() {
        Scan::TokenType operatorType = parser.previous.type;
        ParseRule *rule = getRule(operatorType);
        parsePrecedence((Precedence) (rule->precedence + 1));

        switch (operatorType) {
            case Scan::TOKEN_PLUS: emitByte(Chunk::OP_ADD);
                break;
            case Scan::TOKEN_MINUS: emitByte(Chunk::OP_SUBTRACT);
                break;
            case Scan::TOKEN_STAR: emitByte(Chunk::OP_MULTIPLY);
                break;
            case Scan::TOKEN_SLASH: emitByte(Chunk::OP_DIVIDE);
                break;
            default: return; // Unreachable.
        }
    }

    ParseRule rules[] = {
        [Scan::TOKEN_LEFT_PAREN] = {grouping, nullptr, PREC_NONE},
        [Scan::TOKEN_RIGHT_PAREN] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_LEFT_BRACE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_RIGHT_BRACE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_COMMA] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_DOT] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_MINUS] = {unary, binary, PREC_TERM},
        [Scan::TOKEN_PLUS] = {nullptr, binary, PREC_TERM},
        [Scan::TOKEN_SEMICOLON] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_SLASH] = {nullptr, binary, PREC_FACTOR},
        [Scan::TOKEN_STAR] = {nullptr, binary, PREC_FACTOR},
        [Scan::TOKEN_BANG] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_BANG_EQUAL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_EQUAL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_EQUAL_EQUAL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_GREATER] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_GREATER_EQUAL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_LESS] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_LESS_EQUAL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_IDENTIFIER] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_STRING] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_NUMBER] = {number, nullptr, PREC_NONE},
        [Scan::TOKEN_AND] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_CLASS] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_ELSE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_FALSE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_FOR] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_FUN] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_IF] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_NIL] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_OR] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_PRINT] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_RETURN] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_SUPER] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_THIS] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_TRUE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_VAR] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_WHILE] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_ERROR] = {nullptr, nullptr, PREC_NONE},
        [Scan::TOKEN_EOF] = {nullptr, nullptr, PREC_NONE},
    };

    // parsePrecedence的调用方承诺必须在这个位置要有一个可调用规则
    static void parsePrecedence(Precedence precedence) {
        advance();
        ParseFn prefixRule = getRule(parser.previous.type)->prefix;
        if (prefixRule == nullptr) {
            error("Expect expression.");
            return;
        }
        prefixRule();
        while (precedence <= getRule(parser.current.type)->precedence) {
            advance();
            ParseFn infixRule = getRule(parser.previous.type)->infix;
            infixRule();
        }
    }

    static ParseRule *getRule(Scan::TokenType type) {
        return &rules[type];
    }


    bool compile(const char *source, Chunk::Chunk *chunk) {
        Scan::initScanner(source);

        compilingChunk = chunk;

        parser.hadError = false; // 是否出现错误
        parser.panicMode = false; // 如果有错误，就进入恐慌模式

        advance();
        expression();
        consume(Scan::TOKEN_EOF, "Expect end of expression.");
        endCompiler();
        return !parser.hadError;
    }
}
