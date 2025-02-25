#ifndef DA_H_
#define DA_H_
#include <stdint.h>

#ifndef DA_SIZE_TYPE
#include <stddef.h>
#define DA_SIZE_TYPE size_t
#endif // DA_INT_TYPE

#ifndef DA_SORT
#include <stdlib.h>
#define DA_SORT qsort
#endif

#ifndef DA_ASSERT
#include <assert.h>
#define DA_ASSERT(expr) assert(expr)
#endif // DA_ASSERT

#define DA(Type) Type* items; DA_SIZE_TYPE count; DA_SIZE_TYPE capacity

#define da_with_cap(da, cap) \
    (DA_ASSERT(sizeof((da)->capacity) == sizeof(DA_SIZE_TYPE)), __da_with_cap(da, (uint8_t*)&(da)->items - (uint8_t*)(da), (uint8_t*)&(da)->capacity - (uint8_t*)(da), cap, sizeof((da)->items[0])))

#define da_with_cap2(da, cap) \
    (DA_ASSERT(sizeof((da)->capacity) == sizeof(DA_SIZE_TYPE)), __da_with_cap2(da, (uint8_t*)&(da)->items - (uint8_t*)(da), (uint8_t*)&(da)->capacity - (uint8_t*)(da), cap, sizeof((da)->items[0])))

#define da_append(da, item) \
    (DA_ASSERT(sizeof((da)->count) == sizeof(DA_SIZE_TYPE) && sizeof((da)->capacity) == sizeof(DA_SIZE_TYPE)), __da_append(da, \
            (uint8_t*)&(da)->items - (uint8_t*)(da), \
            (uint8_t*)&(da)->count - (uint8_t*)(da), \
            (uint8_t*)&(da)->capacity - (uint8_t*)(da), item, sizeof(*(item))))

#define da_append_many(da, items_, item_count) \
    (DA_ASSERT(sizeof((da)->count) == sizeof(DA_SIZE_TYPE) && sizeof((da)->capacity) == sizeof(DA_SIZE_TYPE)), __da_append_many(da, \
            (uint8_t*)&(da)->items - (uint8_t*)(da), \
            (uint8_t*)&(da)->count - (uint8_t*)(da), \
            (uint8_t*)&(da)->capacity - (uint8_t*)(da), items_, item_count, sizeof(items_[0])))

void __da_with_cap(void* da, size_t items_field_offset, size_t capacity_field_offset, DA_SIZE_TYPE init_cap, size_t item_size);
void __da_with_cap2(void* da, size_t items_field_offset, size_t capacity_field_offset, DA_SIZE_TYPE init_cap, size_t item_size);
void __da_append(void* da, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* item, size_t item_size);
void __da_append_many(void* da, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* items, size_t item_count, size_t item_size);

#define da_first(da) (DA_ASSERT((da)->count > 0), (da)->items[0])
#define da_last(da) (DA_ASSERT((da)->count > 0), (da)->items[(da)->count - 1])

#define da_pop(da) (DA_ASSERT((da)->count > 0), (da)->items[--(da)->count])
#define da_sort(da, compare_fn) DA_SORT((da)->items, (da)->count, sizeof((da)->items[0]), compare_fn)

#define da_free(da) do { \
        free((da)->items); \
        (da)->items = NULL; \
        (da)->count = 0; \
        (da)->capacity = 0; \
    } while (0)

#define da_foreach(da, Type, ptr) for (Type* ptr = (da)->items; ptr < (da)->items + (da)->count; ptr++)

#endif // DA_H_

#ifdef DA_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef DA_INIT_CAP
    #define DA_INIT_CAP 64
#endif // DA_INIT_CAP

void __da_with_cap(void* da, size_t items_field_offset, size_t capacity_field_offset, DA_SIZE_TYPE init_cap, size_t item_size) {
    void** items = (da + items_field_offset);
    DA_SIZE_TYPE* capacity = (DA_SIZE_TYPE*)(da + capacity_field_offset);

    *capacity = init_cap;
    *items = malloc(*capacity * item_size);
    assert(*items != NULL);
}

void __da_with_cap2(void* da, size_t items_field_offset, size_t capacity_field_offset, DA_SIZE_TYPE init_cap, size_t item_size) {
    void** items = (da + items_field_offset);
    DA_SIZE_TYPE* capacity = (DA_SIZE_TYPE*)(da + capacity_field_offset);

    DA_SIZE_TYPE real_cap = 1;
    while (real_cap < init_cap) real_cap *= 2;

    *capacity = real_cap;
    *items = malloc((*capacity) * item_size);
    assert(*items != NULL);
}

void __da_append(void* da, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* item, size_t item_size) {
    void** items = (void**)(da + items_field_offset);
    DA_SIZE_TYPE* count = (DA_SIZE_TYPE *)(da + count_field_offset);
    DA_SIZE_TYPE* capacity = (DA_SIZE_TYPE *)(da + capacity_field_offset);

    if (*count >= *capacity) { 
        if (*capacity == 0) *capacity = DA_INIT_CAP; 
        while (*count >= *capacity) *capacity *= 2; 
        *items = realloc(*items, *capacity * item_size);
        assert(*items != NULL);
    } 

    memcpy(*items + *count * item_size, item, item_size);
    *count += 1; 
}

void __da_append_many(void* da, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* items, size_t item_count, size_t item_size) {
    void** p_items = (void**)(da + items_field_offset);
    DA_SIZE_TYPE* count = (DA_SIZE_TYPE *)(da + count_field_offset);
    DA_SIZE_TYPE* capacity = (DA_SIZE_TYPE *)(da + capacity_field_offset);

    if (*count + item_count >= *capacity) { 
        if (*capacity == 0) *capacity = DA_INIT_CAP; 
        while (*count + item_count >= *capacity) *capacity *= 2; 
        *p_items = realloc(*p_items, *capacity * item_size);
        assert(*p_items != NULL);
    } 

    memcpy(*p_items + *count * item_size, items, item_count * item_size);
    *count += item_count; 
}
#endif
