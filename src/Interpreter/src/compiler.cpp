//
// Created by 15813 on 2026/8/15.
//
#include <cstdio>
#include "common.h"
#include "compiler.h"
#include <cstdlib>
#include "scanner.h"

namespace CLOX::Compiler{

    typedef struct {
        Scan::Token current;
        Scan::Token previous;
        bool hadError;
        bool panicMode;
    } Parser;

    Parser parser;
    Chunk::Chunk* compilingChunk;

    static Chunk::Chunk* currentChunk() {
        return compilingChunk;
    }

    static void errorAt(Scan::Token* token, const char* message) {
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

    static void error(const char* message) {
        errorAt(&parser.previous, message);
    }

    static void errorAtCurrent(const char* message) {
        errorAt(&parser.current, message);
    }

    static void advance() {
        parser.previous = parser.current;

        for (;;) {
            parser.current = Scan::scanToken();
            if (parser.current.type != Scan::TOKEN_ERROR) break;

            errorAtCurrent(parser.current.start);
        }
    }

    static void consume(Scan::TokenType type, const char* message) {
        if (parser.current.type == type) {
            advance();
            return;
        }
        errorAtCurrent(message);
    }

    static void emitByte(uint8_t byte) {
        writeChunk(currentChunk(), byte, parser.previous.line);
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
    }

    static void expression() {

    }

    bool compile(const char* source, Chunk::Chunk* chunk) {
        Scan::initScanner(source);

        compilingChunk = chunk;

        parser.hadError = false;
        parser.panicMode = false;

        advance();
        expression();
        consume(Scan::TOKEN_EOF, "Expect end of expression.");
        endCompiler();
        return !parser.hadError;
    }
}
