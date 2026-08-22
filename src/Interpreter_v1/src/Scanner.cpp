//
// Created by 15813 on 2026/8/21.
//

#include "Scanner.h"
#include "error.h"

namespace CLOX_V1::Scanner{
    std::map<std::string, Token::TokenType_t> ScannerSystem::keywords = {};

    ScannerSystem::ScannerSystem(std::string source) : source(std::move(source)){
        if (keywords.empty()){
            keywords.insert(std::make_pair("and", Token::AND));
            keywords.insert(std::make_pair("class", Token::CLASS));
            keywords.insert(std::make_pair("else", Token::ELSE));
            keywords.insert(std::make_pair("false", Token::FALSE));
            keywords.insert(std::make_pair("for", Token::FOR));
            keywords.insert(std::make_pair("fun", Token::FUN));
            keywords.insert(std::make_pair("if", Token::IF));
            keywords.insert(std::make_pair("nil", Token::NIL));
            keywords.insert(std::make_pair("or", Token::OR));
            keywords.insert(std::make_pair("print", Token::PRINT));
            keywords.insert(std::make_pair("return", Token::RETURN));
            keywords.insert(std::make_pair("super", Token::SUPER));
            keywords.insert(std::make_pair("this", Token::THIS));
            keywords.insert(std::make_pair("true", Token::TRUE));
            keywords.insert(std::make_pair("var", Token::VAR));
            keywords.insert(std::make_pair("while", Token::WHILE));
        }
    }

    std::vector<Token::Token> ScannerSystem::scanTokens() {
        while (!isAtEnd()){
            start = current;
            scanToken();
        }

        tokens.emplace_back(Token::EOF_Type, "", std::string(), line);

        return tokens;
    }

    bool ScannerSystem::isAtEnd() {
        return current >= source.length();
    }

    void ScannerSystem::scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(Token::LEFT_PAREN);
                break;
            case ')': addToken(Token::RIGHT_PAREN);
                break;
            case '{': addToken(Token::LEFT_BRACE);
                break;
            case '}': addToken(Token::RIGHT_BRACE);
                break;
            case ',': addToken(Token::COMMA);
                break;
            case '.': addToken(Token::DOT);
                break;
            case '-': addToken(Token::MINUS);
                break;
            case '+': addToken(Token::PLUS);
                break;
            case ';': addToken(Token::SEMICOLON);
                break;
            case '*': addToken(Token::STAR);
                break;
            case '!': addToken(match('=') ? Token::BANG_EQUAL : Token::BANG);
                break;
            case '=': addToken(match('=') ? Token::EQUAL_EQUAL : Token::EQUAL);
                break;
            case '<': addToken(match('=') ? Token::LESS_EQUAL : Token::LESS);
                break;
            case '>': addToken(match('=') ? Token::GREATER_EQUAL : Token::GREATER);
            case '/':
                if (match('/')){
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (match('*')){
                    comment();
                }
                else{
                    addToken(Token::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;
            case '"': string();
                break;
            default:
                if (isDigit(c)){
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                }else {
                    Err::error(line, "Unexpected character.");
                    hadError = true;
                }
                break;
        }
    }

    void ScannerSystem::identifier() {
        while (isAlphaNumeric(peek())) advance();

        std::string text = source.substr(start, current - start);
        auto it = keywords.find(text);
        if (it != keywords.end()) addToken(it->second);
        else addToken(Token::IDENTIFIER);
    }

    char ScannerSystem::advance() {
        current++;
        return source.at(current - 1);
    }

    void ScannerSystem::addToken(Token::TokenType_t type) {
        addToken(type, std::string());
    }

    void ScannerSystem::addToken(Token::TokenType_t type, const std::any literal) {
        std::string text = source.substr(start, current - start);
        tokens.emplace_back(type, text, literal, line);
    }

    bool ScannerSystem::match(char expected) {
        if (isAtEnd()) return false;
        if (source.at(current) != expected) return false;
        current++;
        return true;
    }

    char ScannerSystem::peek() {
        if (isAtEnd()) return '\0';
        return source.at(current);
    }

    void ScannerSystem::string() {
        while (peek() != '"' && !isAtEnd()){
            if (peek() == '\n') line ++;
            advance();
        }

        if (isAtEnd()){
            Err::error(line, "Unterminated string.");
            hadError = true;
            return;
        }

        advance();

        std::string value = source.substr(start+1, current - 1 - start - 1);
        addToken(Token::STRING, value);
    }

    bool ScannerSystem::isDigit(char c) {
        return std::isdigit(c);
    }

    void ScannerSystem::number() {
        while (isDigit(peek())) advance();

        if (peek() == '.' && isDigit(peekNext())){
            advance();
            while (isDigit(peek())) advance();
        }

        std::string temp = source.substr(start, current - start + 1);
        addToken(Token::NUMBER, static_cast<double>(std::atof(temp.c_str())));
    }

    char ScannerSystem::peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source.at(current + 1);
    }

    bool ScannerSystem::isAlpha(char c) {
        return std::isalpha(c) || c == '_';
    }

    bool ScannerSystem::isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    void ScannerSystem::comment() {
        auto is_out = [&]()->bool{
            return current < source.length() && peek() == '*' && current + 1 < source.length() && peekNext() == '/';
        };
        auto is_in = [&]()->bool{
            return current < source.length() && peek() == '/' && current + 1 < source.length() && peekNext() == '*';
        };
        int comment_level = 1;

        while (comment_level > 0){
            if (is_out()){
                advance(); advance();
                comment_level --;
            }else if (is_in()){
                advance(); advance();
                comment_level ++;
            }else if(peek() == '\n'){
                advance();
                line ++;
            }else{
                advance();
            }
        }

        std::string value = source.substr(start, current - start);
        addToken(Token::COMMENTS, value);
    }
}