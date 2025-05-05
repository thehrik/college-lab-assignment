#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../result/result.h"

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

ResultString String_read_line(String *s)
{
  while (1)
  {
    int c = getchar();
    if (c == '\n')
    {
      break; // End of line
    }
    if (c == EOF)
    {
      if (s->length == 0)
      {
        // No data read, true EOF
        return (ResultString){RESULT_ERR, "EOF reached without reading any data"};
      }
      else
      {
        // Return what we have, but signal EOF
        break;
      }
    }
    char ch[2];
    ch[0] = (char)c;
    ch[1] = '\0';
    if (!String_append(s, ch))
    {
      return (ResultString){RESULT_ERR, "Memory allocation failed"};
    }
  }
  return (ResultString){RESULT_OK, NULL}; // Success
}
