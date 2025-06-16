#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../result/result.h"
#include "../types/types.h"

String *String_new(size_t init_capacity)
{
  String *s = malloc(sizeof(String));
  if (!s)
    return NULL;
  s->data = malloc(init_capacity);
  if (!s->data)
  {
    free(s);
    return NULL;
  }
  s->length = 0;
  s->capacity = init_capacity;
  s->data[0] = '\0';
  return s;
}

int String_append(String *s, const char *suffix)
{
  size_t suffix_len = strlen(suffix);
  size_t new_length = s->length + suffix_len;
  if (new_length + 1 > s->capacity)
  {
    size_t new_capacity = (new_length + 1) * 2;
    char *new_data = realloc(s->data, new_capacity);
    if (!new_data)
      return 0; // Allocation failed
    s->data = new_data;
    s->capacity = new_capacity;
  }
  memcpy(s->data + s->length, suffix, suffix_len + 1); // +1 for null terminator
  s->length = new_length;
  return 1;
}

void String_destroy(String *s)
{
  if (s)
  {
    free(s->data);
    free(s);
  }
}

Result String_read_line(String *s)
{
  if (!s)
    return (Result){ERR, .data.err_str = "Null String pointer"};

  s->length = 0; // Reset the string for new input
  if (s->capacity == 0)
  {
    s->data = malloc(16);
    if (!s->data)
      return (Result){ERR, .data.err_str = "Memory allocation failed"};
    s->capacity = 16;
    s->data[0] = '\0';
  }

  while (1)
  {
    int c = getchar();
    if (c == '\n' || c == EOF)
      break;

    char ch[2] = {(char)c, '\0'};
    if (!String_append(s, ch))
      return (Result){ERR, .data.err_str = "Memory allocation failed"};
  }

  if (s->length == 0 && feof(stdin))
    return (Result){ERR, .data.err_str = "EOF reached without reading any data"};
  char *data = s->data;
  return (Result){OK, .data.ok = data};
}