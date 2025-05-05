#include "parser.h"

#include "../result/result.h" // or "parser.h" if it includes result.h
#include <stddef.h>           // for NULL

ResultInt parse_to_int(const char *str)
{
  if (str == NULL || *str == '\0')
  {
    return (ResultInt){RESULT_ERR, 0, "Input string is empty"};
  }

  int sign = 1;
  if (*str == '-')
  {
    sign = -1;
    str++;
  }

  if (*str < '0' || *str > '9')
  {
    return (ResultInt){RESULT_ERR, 0, "No digits found after sign"};
  }

  int num = 0;
  while (*str >= '0' && *str <= '9')
  {
    num = num * 10 + (*str - '0');
    str++;
  }

  // If there are any non-digit characters left, it's an error
  if (*str != '\0')
  {
    return (ResultInt){RESULT_ERR, 0, "Invalid character in input"};
  }

  return (ResultInt){RESULT_OK, sign * num, NULL};
}
