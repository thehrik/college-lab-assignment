#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h> // for size_t
#include "../result/result.h"

typedef struct
{
  int *data;
  size_t length;
  size_t capacity;
} VecInt;

VecInt *VecInt_new(size_t init_capacity);
ResultVecInt VecInt_append(VecInt *vec, int value);
void VecInt_destroy(VecInt *s);

#endif // VECTOR_H