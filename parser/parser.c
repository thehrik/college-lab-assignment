#include <limits.h>
#include <stddef.h>
#include "../result/result.h"

ResultInt parse_to_int(const char *str)
{
  if (str == NULL || *str == '\0')
    return (ResultInt){RESULT_ERR, .data.err = "Input string is empty"};

  int sign = 1;
  if (*str == '-')
  {
    sign = -1;
    str++;
  }

  if (*str < '0' || *str > '9')
    return (ResultInt){RESULT_ERR, .data.err = "No digits found after sign"};

  int num = 0;
  while (*str >= '0' && *str <= '9')
  {
    int digit = *str - '0';

    if (sign == 1)
    {
      if (num > (INT_MAX - digit) / 10)
        return (ResultInt){RESULT_ERR, .data.err = "Number too large"};
    }
    else
    {
      // For negative numbers, check INT_MIN
      if (num < (INT_MIN + digit) / 10)
        return (ResultInt){RESULT_ERR, .data.err = "Number too small"};
    }

    num = num * 10 + sign * digit;
    str++;
  }

  if (*str != '\0')
    return (ResultInt){RESULT_ERR, .data.err = "Invalid character in input string"};

  return (ResultInt){RESULT_OK, .data.ok = num};
}
