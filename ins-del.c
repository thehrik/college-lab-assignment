#include <stdio.h>
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
  int value;         // The parsed integer (valid if status == RESULT_OK)
  const char *error; // Error message (valid if status == RESULT_ERR)
} ResultGetInt;

// type definitions for get result of vector of integers
typedef struct
{
  GetResultStatus status; // The parsed vector (valid if status == RESULT_OK)
  const char *error;      // Error message (valid if status == RESULT_ERR)
} ResultGetVecInt;

// Function prototypes
ResultGetInt get_int();
ResultGetVecInt get_vec_int(VecInt *vec);

// Main function
int main()
{
  VecInt *vec = VecInt_new(0);

  ResultGetVecInt r = get_vec_int(vec);
  if (r.status == GET_ERR)
  {
    fprintf(stderr, "Error: %s\n", r.error);
    VecInt_destroy(vec); // Free memory before exiting
    return 1;
  }
  else if (r.status == GET_STOPPED)
  {
    printf("Input stopped by user.\n");
  }

  for (int i = 0; i < vec->length; i++)
  {
    printf("%d\n", vec->data[i]);
  }
}

// function to get integer input from user
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
    printf("enter value: ");
    ResultString rs = String_read_line(s);
    if (rs.status == RESULT_ERR)
    {
      fprintf(stderr, "Error: %s\n", rs.data.err);
      String_destroy(s); // Free memory before continuing
      continue;
    }
    else
    {
      if (*s->data == 'q')
      {
        String_destroy(s); // Free memory before breaking
        return (ResultGetInt){GET_STOPPED, 0, NULL};
        break;
      }
      ResultInt ri = parse_to_int(s->data);
      if (ri.status == RESULT_ERR)
      {
        fprintf(stderr, "Error: %s\n", ri.data.err);
        String_destroy(s); // Free memory before continuing
        continue;
      }
      else
      {
        result = ri.data.value;
        String_destroy(s); // Free memory before breaking
        break;
      }
    }
  }
  return (ResultGetInt){GET_OK, result, NULL};
}

// function to get vector of integers from user
ResultGetVecInt get_vec_int(VecInt *vec)
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
      break;
    }
    else if (r.status == GET_OK)
    {
      printf("Parsed value: %d\n", r.value);
      ResultVecInt rv = VecInt_append(vec, r.value);
      if (rv.status == RESULT_ERR)
      {
        fprintf(stderr, "Error: %s\n", rv.data.err);
        continue;
      }
    }
  }
}