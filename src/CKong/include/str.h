//
// Created by YangKai.Lin on 2026/7/28.
//

#ifndef TOYPROJECTS_CKONG_SRC_H
#define TOYPROJECTS_CKONG_SRC_H
#include <cstdint>
#include <cstdio>

namespace CKong {
    typedef struct {
        uint16_t len; // short
        uint16_t pos; // short
        uint8_t *data; // char
    }str_t;

    void str_free(str_t *str);

    str_t *str_new(uint16_t len);

    uint16_t str_pos(const str_t *str);

    uint16_t str_size(const str_t *str);

    str_t* str_clone(const char* value);

    void str_printc(char c, FILE* file);

    void str_print(const str_t* str, FILE* file);

    str_t* str_left(const str_t* lhs, uint16_t len);

    str_t* str_right(const str_t* lhs, uint16_t len);

    uint16_t str_appendz(str_t* str, const char* lhs);

    uint16_t str_append(str_t* str, const str_t* lhs);

    str_t* str_concat(const str_t* lhs, const str_t* rhs);
}
#endif //TOYPROJECTS_CKONG_SRC_H