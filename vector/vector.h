#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h> // for size_t
#include "../result/result.h"
#include "../types/types.h"

Vec *vec_new(size_t elem_size, size_t init_capacity);
int vec_append(Vec *vec, void *elem);
void *vec_get(Vec *vec, size_t index);
void vec_destroy(Vec *vec);

#endif // VECTOR_H