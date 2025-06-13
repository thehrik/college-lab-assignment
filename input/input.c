#include "../result/result.h"
#include "../types/types.h"
#include "../string/string.h"
#include "../parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ReadResult int_read_line()
{
  String *s = String_new(0);
  if (!s){
    return (ReadResult){READ_ERR, .data.err_str = "Memory allocation failed"};
  }
  Result ri;
  while (1)
  {
    printf("Enter integer: ");
    Result rs = String_read_line(s);
    if (rs.status == ERR)
    {
      fprintf(stderr, "Error: %s\n", rs.data.err_str);
      String_destroy(s); // Free memory before continuing
      continue;
    }
    else
    {
      if (*s->data == 'q')
      {
        String_destroy(s); // Free memory before breaking
        return (ReadResult){READ_STOPPED, .data.ok = NULL};
      }
      ri = parse_to_int(s->data);
      if (ri.status == ERR)
      {
        fprintf(stderr, "Error: %s\n", ri.data.err_str);
        continue;
      }
      else
      {
        String_destroy(s); // Free memory before breaking
        break;
      }
    }
  }
  return (ReadResult){READ_OK, .data.ok = ri.data.ok};
}