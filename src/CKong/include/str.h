//
// Created by 15813 on 2026/7/21.
//

#ifndef TOYPROJECTS_CKONG_HELPER_H
#define TOYPROJECTS_CKONG_HELPER_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>

namespace CKong::Helper {
    typedef struct cstring {
        uint16_t len;
        uint8_t *data;
    } cstring_t;

    cstring_t *string_clone(const char *str);
    void string_free(cstring_t* string);
    cstring_t* string_new(uint16_t len);
    void string_print(cstring_t* string, FILE* file);
    uint16_t string_len(cstring_t* string);
    cstring_t* string_concat(const cstring_t* lhs, const cstring_t* rhs);
    cstring_t* string_left(const cstring_t* lhs, uint16_t len);
    cstring_t* string_right(const cstring_t* rhs, uint16_t len);
}
#endif //TOYPROJECTS_CKONG_HELPER_H