#include "vector.h"
#include <stdlib.h>
#include "../result/result.h"
#include "../types/types.h"

VecInt *VecInt_new(size_t init_capacity)
{
  VecInt *s = malloc(sizeof(VecInt));
  if (!s)
    return NULL;
  s->data = malloc(init_capacity * sizeof(int));
  if (!s->data)
  {
    free(s);
    return NULL;
  }
  s->length = 0;
  s->capacity = init_capacity;
  return s;
}

ResultVecInt VecInt_append(VecInt *vec, int value)
{
  if (vec->length >= vec->capacity)
  {
    size_t new_capacity = (vec->capacity == 0) ? 1 : vec->capacity * 2;
    int *new_data = realloc(vec->data, new_capacity * sizeof(int));
    if (!new_data)
      return (ResultVecInt){RESULT_ERR, .data.err = "Memory allocation failed"};
    vec->data = new_data;
    vec->capacity = new_capacity;
  }
  vec->data[vec->length++] = value;
  return (ResultVecInt){RESULT_OK, .data.ok = vec};
}

void VecInt_destroy(VecInt *vec)
{
  if (vec)
  {
    free(vec->data);
    free(vec);
  }
}