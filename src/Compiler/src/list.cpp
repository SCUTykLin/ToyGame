//
// Created by 15813 on 2026/8/12.
//

#include "../include/list.h"

namespace Compiler::list{
    list_T *init_list(size_t item_size){
        list_T* list = static_cast<list_T*>(calloc(1, sizeof(struct LIST_STRUCT)));
        list->size = 0;
        list->item_size = item_size;
        list->items = nullptr;
        return list;
    }

    void list_push(list_T* list, void* item){
        list->size += 1;
        if (!list->items){
            list->items = static_cast<void**>(calloc(1, list->item_size));
        } else{
            list->items = static_cast<void**>(realloc(list->items, (list->size * list->item_size)));
        }
        list->items[list->size - 1] = item;
    }
}