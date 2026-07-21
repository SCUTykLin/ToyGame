//
// Created by 15813 on 2026/7/21.
//

#include "../include/str.h"
#include <cstring>
#include <cassert>

namespace CKong::Helper{
    static char buffer[65536]; // Note: not thread safe
    cstring_t *string_clone(const char *str){
        cstring_t* string = (cstring_t*)malloc(sizeof(cstring_t));
        if (string == nullptr){
            abort();
        }
        string->data = (uint8_t*)_strdup(str);
        string->len = (uint16_t) strlen(str);
        return string;
    }

    cstring_t* string_new(uint16_t len){
        cstring_t* string = (cstring_t*)malloc(sizeof(cstring_t));
        if (string == nullptr){
            abort();
        }
        string->data = (uint8_t*) malloc(len);
        string->len = len;
        return string;
    }

    void string_free(cstring_t* string){
        assert(string != nullptr);
        assert(string->data != nullptr);
        free(string);
    }

    void string_print(cstring_t* string, FILE* file){
        assert(string != nullptr);
        assert(file != nullptr);
        memcpy(&buffer, string->data, string->len);
        buffer[string->len] = '\0';
        fprintf(file, "%s", buffer);
    }

    uint16_t string_len(cstring_t* string){
        assert(string != nullptr);
        return string->len;
    }

    cstring_t* string_concat(const cstring_t* lhs, const cstring_t* rhs){
        assert(lhs != nullptr);
        assert(rhs != nullptr);
        cstring_t* string = (cstring_t*)malloc(sizeof(cstring_t));
        string->data = (uint8_t*) malloc(lhs->len + rhs->len);
        string->len = lhs->len + rhs->len;
        memcpy(string->data, lhs->data, lhs->len);
        memcpy(string->data + lhs->len, rhs->data, rhs->len);
        return string;
    }

    cstring_t* string_left(const cstring_t* lhs, uint16_t len){
        assert(lhs != nullptr);
        assert(len > 0 && len <= lhs->len);
        cstring_t* string = (cstring_t*) malloc(sizeof(cstring_t));
        string->data = (uint8_t*) malloc(len);
        string->len = len;
        memcpy(string->data, lhs->data, len);
        return string;
    }

    cstring_t* string_right(const cstring_t* rhs, uint16_t len){
        assert(rhs != nullptr);
        assert(len > 0 && len <= rhs->len);
        cstring_t* string = (cstring_t*) malloc(sizeof(cstring_t));
        string->data = (uint8_t*) malloc(len);
        string->len = len;
        memcpy(string->data, rhs->data + rhs->len - 1 - len, len);
        return string;
    }

}