//
// Created by 15813 on 2026/8/12.
//

#ifndef TOYPROJECTS_COMPILER_LIST_H
#define TOYPROJECTS_COMPILER_LIST_H

#include <cstdlib>
namespace Compiler::list {
    typedef struct LIST_STRUCT {
        void **items;
        size_t size;
        size_t item_size;
    } list_T;

    list_T *init_list(size_t item_size);

    void list_push(list_T* list, void* item);
}
#endif //TOYPROJECTSv_LIST_H
