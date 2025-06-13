#include "vector.h"
#include <stdlib.h>
#include <string.h>
#include "../result/result.h"
#include "../types/types.h"

Vec *vec_new(size_t elem_size, size_t init_capacity)
{
  Vec *vec = malloc(sizeof(Vec));
  if (!vec)
    return NULL;
  vec->data = malloc(elem_size * init_capacity);
  if (!vec->data)
  {
    free(vec);
    return NULL;
  }
  vec->length = 0;
  vec->capacity = init_capacity;
  vec->elem_size = elem_size;
  return vec;
}

int vec_append(Vec *vec, void *elem)
{
  if (vec->length >= vec->capacity)
  {
    size_t new_capacity = (vec->capacity == 0) ? 1 : vec->capacity * 2;
    void *new_data = realloc(vec->data, new_capacity * vec->elem_size);
    if (!new_data)
      return 0; // failure
    vec->data = new_data;
    vec->capacity = new_capacity;
  }
  memcpy((char *)vec->data + vec->length * vec->elem_size, elem, vec->elem_size);
  vec->length++;
  return 1; // success
}

void *vec_get(Vec *vec, size_t index)
{
  if (index >= vec->length)
    return NULL;
  return (char *)vec->data + index * vec->elem_size;
}

void vec_destroy(Vec *vec)
{
  if (vec)
  {
    free(vec->data);
    free(vec);
  }
}