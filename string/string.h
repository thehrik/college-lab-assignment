#ifndef STRING_H
#define STRING_H

#include <stddef.h> // for size_t
#include "../result/result.h"
#include "../types/types.h"

String *String_new(size_t init_capacity);
int String_append(String *s, const char *suffix);
void String_destroy(String *s);
//ResultString String_read_line(String *s);
Result String_read_line(String *s);

#endif // STRING_H