#include <stdio.h>
#include "./string/string.h"
#include "./result/result.h"
#include "./parser/parser.h"
#include "./vector/vector.h"

typedef enum
{
  GET_INT_OK,
  GET_INT_ERR,
  GET_INT_STOPPED,
} GetIntResultStatus;

typedef struct
{
  GetIntResultStatus status;
  int value;         // The parsed integer (valid if status == RESULT_OK)
  const char *error; // Error message (valid if status == RESULT_ERR)
} ResultGetInt;

ResultGetInt get_int();

int main()
{
  VecInt *vec = VecInt_new(0);

  while (1)
  {
    ResultGetInt r = get_int();
    if (r.status == GET_INT_ERR)
    {
      fprintf(stderr, "Error: %s\n", r.error);
      continue;
    }
    else if (r.status == GET_INT_STOPPED)
    {
      break;
    }
    else if (r.status == GET_INT_OK)
    {
      printf("Parsed value: %d\n", r.value);
      ResultVecInt rv = VecInt_append(vec, r.value);
      if (rv.status == RESULT_ERR)
      {
        fprintf(stderr, "Error: %s\n", rv.error);
        continue;
      }
    }
  }

  for (int i = 0; i < vec->length; i++)
  {
    printf("%d\n", vec->data[i]);
  }

}

ResultGetInt get_int()
{
  int result;

  while (1)
  {
    String *s = String_new(0);
    if (!s)
    {
      return (ResultGetInt){GET_INT_ERR, 0, "memory allocation failed\n"};
    }
    printf("enter value: ");
    ResultString rs = String_read_line(s);
    if (rs.status == RESULT_ERR)
    {
      fprintf(stderr, "Error: %s\n", rs.error);
      String_destroy(s); // Free memory before continuing
      continue;
    }
    else
    {
      if (*s->data == 'q')
      {
        String_destroy(s); // Free memory before breaking
        return (ResultGetInt){GET_INT_STOPPED, 0, NULL};
        break;
      }
      ResultInt ri = parse_to_int(s->data);
      if (ri.status == RESULT_ERR)
      {
        fprintf(stderr, "Error: %s\n", ri.error);
        String_destroy(s); // Free memory before continuing
        continue;
      }
      else
      {
        result = ri.value;
        String_destroy(s); // Free memory before breaking
        break;
      }
    }
  }
  return (ResultGetInt){GET_INT_OK, result, NULL};
}
