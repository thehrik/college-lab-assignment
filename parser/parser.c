#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include "../result/result.h"

Result parse_to_int(const char *str)
{
  if (str == NULL || *str == '\0')
    return (Result){ERR, .data.err_str = "Input string is empty"};

  int sign = 1;
  if (*str == '-')
  {
    sign = -1;
    str++;
  }

  if (*str < '0' || *str > '9')
    return (Result){ERR, .data.err_str = "No digits found after sign"};

  int num = 0;
  while (*str >= '0' && *str <= '9')
  {
    int digit = *str - '0';

    if (sign == 1)
    {
      if (num > (INT_MAX - digit) / 10)
        return (Result){ERR, .data.err_str = "Number too large"};
    }
    else
    {
      // For negative numbers, check INT_MIN
      if (num < (INT_MIN + digit) / 10)
        return (Result){ERR, .data.err_str = "Number too small"};
    }

    num = num * 10 + sign * digit;
    str++;
  }

  if (*str != '\0')
    return (Result){ERR, .data.err_str = "Invalid character in input string"};

  int *ptr = malloc(sizeof(int));
  *ptr = num;
  return (Result){OK, .data.ok = ptr};
}
