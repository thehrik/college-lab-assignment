#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h"
#include "./result/result.h"
#include "./parser/parser.h"
#include "./vector/vector.h"
#include "./input/input.h"

void main()
{
  // String *s = String_new(0);
  // if (!s)
  // {
  //   fprintf(stderr, "Error: Memory allocation failed\n");
  //   return;
  // }
  // Result r = String_read_line(s);
  // printf("%s", s->data);
  // printf("%s", (char*)r.data.ok);
  ReadResult ri = int_read_line();
  printf("%d", *(int *)ri.data.ok);

  return;
}