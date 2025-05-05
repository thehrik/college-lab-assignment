#ifndef STRING_H
#define STRING_H

#include <stddef.h> // for size_t
#include "../result/result.h" 

typedef struct
{
    char *data;
    size_t length;
    size_t capacity;
} String;

String *String_new(size_t init_capacity);
int String_append(String *s, const char *suffix);
void String_destroy(String *s);
ResultString String_read_line(String *s);

#endif // STRING_H