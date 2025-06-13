#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h"
#include "./result/result.h"
#include "./parser/parser.h"
#include "./vector/vector.h"

// type definitions for the results
typedef enum
{
  GET_OK,
  GET_ERR,
  GET_STOPPED,
} GetResultStatus;

// type definitions for get result of integer
typedef struct
{
  GetResultStatus status;
  int value;         // The parsed integer (valid if status == OK)
  const char *error; // Error message (valid if status == ERR)
} ResultGetInt;

// type definitions for get result of vector of integers
typedef struct
{
  GetResultStatus status; // The parsed vector (valid if status == OK)
  const char *error;      // Error message (valid if status == ERR)
} ResultGetVecInt;

// Function prototypes
ResultGetInt get_int();
ResultGetVecInt get_vec_int(Vec *vec);
int get_index(Vec *vec, int el);

int main()
{
  Vec *vec = vec_new(sizeof(int), 0);

  ResultGetVecInt r = get_vec_int(vec);
  if (r.status == GET_ERR)
  {
    fprintf(stderr, "Error: %s\n", r.error);
    vec_destroy(vec); // Free memory before exiting
    return 1;
  }
  else if (r.status == GET_STOPPED)
  {
    printf("Input stopped by user.\n");
  }

  for (size_t i = 0; i < vec->length; i++)
  {
    printf("%d\n", *(int *)vec_get(vec, i));
  }

  while (1)
  {
    printf("Enter value to search (q to quit): ");
    ResultGetInt value = get_int();
    if (value.status == GET_ERR)
    {
      fprintf(stderr, "Error: %s\n", value.error);
      continue;
    }
    else if (value.status == GET_STOPPED)
    {
      vec_destroy(vec);
      return 0;
    }
    printf("Found at index %d\n", get_index(vec, value.value));
  }
}

int get_index(Vec *vec, int el)
{
  for (size_t i = 0; i < vec->length; i++)
  {
    if (*(int *)vec_get(vec, i) == el)
    {
      return (int)i;
    }
  }
  return -1; // Not found
}

ResultGetInt get_int()
{
  int result;

  while (1)
  {
    String *s = String_new(0);
    if (!s)
    {
      return (ResultGetInt){GET_ERR, 0, "memory allocation failed\n"};
    }
    printf("enter integer: ");
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
        return (ResultGetInt){GET_STOPPED, 0, NULL};
      }
      Result ri = parse_to_int(s->data);
      if (ri.status == ERR)
      {
        fprintf(stderr, "Error: %s\n", ri.data.err_str);
        String_destroy(s); // Free memory before continuing
        continue;
      }
      else
      {
        result = ri.data.ok;
        String_destroy(s); // Free memory before breaking
        break;
      }
    }
  }
  return (ResultGetInt){GET_OK, result, NULL};
}

// function to get vector of integers from user
ResultGetVecInt get_vec_int(Vec *vec)
{
  while (1)
  {
    ResultGetInt r = get_int();
    if (r.status == GET_ERR)
    {
      fprintf(stderr, "Error: %s\n", r.error);
      continue;
    }
    else if (r.status == GET_STOPPED)
    {
      return (ResultGetVecInt){GET_STOPPED, NULL};
    }
    else if (r.status == GET_OK)
    {
      if (!vec_append(vec, &r.value))
      {
        fprintf(stderr, "Error: Memory allocation failed\n");
        continue;
      }
    }
  }
}
