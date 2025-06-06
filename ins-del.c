#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
ResultGetVecInt get_vec_int(Vec *vec);
void insert_at_position(Vec *vec, int value, int pos);
void delete_at_position(Vec *vec, int pos);
void print_array(Vec *vec);

// Main function
int main()
{
  int pos, value;
  Vec *vec = vec_new(sizeof(int), 0);

  ResultGetVecInt r = get_vec_int(vec);
  if (r.status == GET_ERR)
  {
    fprintf(stderr, "Error: %s\n", r.error);
    vec_destroy(vec);
    return 1;
  }
  else if (r.status == GET_STOPPED)
  {
    printf("Input stopped by user.\n");
  }

  for (size_t i = 0; i < vec->length; i++)
  {
    printf("| %d ", *(int *)vec_get(vec, i));
  }
  printf("|\n");

  while (1)
  {
    printf("\n1. Insert\n2. Delete\n3. Print\n4. Exit\n");
    printf("Enter your choice: ");
    int choice = get_int().value;

    switch (choice)
    {
    case 1:
      printf("Enter value to insert: ");
      value = get_int().value;
      printf("Enter position (0-based index): ");
      pos = get_int().value;
      insert_at_position(vec, value, pos);
      break;
    case 2:
      printf("Enter position to delete (0-based index): ");
      pos = get_int().value;
      delete_at_position(vec, pos);
      break;
    case 3:
      print_array(vec);
      break;
    case 4:
      printf("Exiting.\n");
      vec_destroy(vec);
      return 0;
    default:
      printf("Invalid choice.\n");
    }
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
    printf("enter integer: ");
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

void insert_at_position(Vec *vec, int value, int pos)
{
  if (pos < 0 || (size_t)pos > vec->length)
  {
    printf("Invalid position.\n");
    return;
  }

  // Ensure there is enough capacity
  if (vec->length >= vec->capacity)
  {
    size_t new_capacity = (vec->capacity == 0) ? 1 : vec->capacity * 2;
    void *new_data = realloc(vec->data, new_capacity * vec->elem_size);
    if (!new_data)
    {
      printf("Memory allocation failed.\n");
      return;
    }
    vec->data = new_data;
    vec->capacity = new_capacity;
  }

  // Shift elements to the right to make space
  for (size_t i = vec->length; i > (size_t)pos; i--)
  {
    memcpy((char *)vec->data + i * vec->elem_size,
           (char *)vec->data + (i - 1) * vec->elem_size,
           vec->elem_size);
  }

  // Insert the new value
  memcpy((char *)vec->data + pos * vec->elem_size, &value, vec->elem_size);
  vec->length++;

  printf("Inserted %d at position %d.\n", value, pos);
}

void delete_at_position(Vec *vec, int pos)
{
  if (vec->length == 0)
  {
    printf("Array is empty. Cannot delete.\n");
    return;
  }
  if (pos < 0 || (size_t)pos >= vec->length)
  {
    printf("Invalid position.\n");
    return;
  }

  // Shift elements to the left to overwrite the deleted element
  for (size_t i = pos; i < vec->length - 1; i++)
  {
    memcpy((char *)vec->data + i * vec->elem_size,
           (char *)vec->data + (i + 1) * vec->elem_size,
           vec->elem_size);
  }
  vec->length--;

  printf("Deleted element at position %d.\n", pos);
}

void print_array(Vec *vec)
{
  printf("Array length: %zu\n", vec->length);
  for (size_t i = 0; i < vec->length; i++)
  {
    printf("%d ", *(int *)vec_get(vec, i));
  }
  printf("\n");
}