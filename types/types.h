#ifndef TYPES_H
#define TYPES_H

#include <stddef.h> // for size_t

typedef struct {
  void *data;
  size_t length;
  size_t capacity;
  size_t elem_size;
} Vec;

typedef struct String
{
  char *data;
  size_t length;
  size_t capacity;
} String;


#endif // TYPES_H
