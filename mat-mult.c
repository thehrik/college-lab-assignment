#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h"
#include "./result/result.h"
#include "./parser/parser.h"
#include "./vector/vector.h"

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

typedef struct
{
  Vec *rows;
  int nrows;
  int ncols;
} Mat;

// Function prototypes
ResultGetInt get_int();
Mat *mat_new(int rows, int cols);
void mat_input(Mat *mat);
void print_mat(Mat *mat);
int matrix_get(Mat *mat, int i, int j);
void matrix_set(Mat *mat, int i, int j, int value);
Mat *mat_mult(Mat *mat1, Mat *mat2);
void mat_destroy(Mat *mat);

// Main function
int main()
{
  int rows, cols;
  printf("Matrix addition program\n");
  printf("Enter the rows and columns of the first matrix: \n");
  printf("Enter number of rows:");
  rows = get_int().value;
  printf("Enter number of columns: ");
  cols = get_int().value;
  Mat *mat1 = mat_new(rows, cols);
  if (!mat1)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return 1;
  }
  printf("Enter the elements of the first matrix:\n");
  mat_input(mat1);

  printf("Enter the rows and columns of the second matrix: \n");
  printf("Enter number of rows:");
  rows = get_int().value;
  printf("Enter number of columns: ");
  cols = get_int().value;
  Mat *mat2 = mat_new(rows, cols);
  if (!mat2)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    free(mat1->rows);
    free(mat1);
    return 1;
  }
  printf("Enter the elements of the second matrix:\n");
  mat_input(mat2);

  Mat *result = mat_mult(mat1, mat2);
  if (!result)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    free(mat1->rows);
    free(mat1);
    free(mat2->rows);
    free(mat2);
    return 1;
  }
  print_mat(result);
  mat_destroy(result); // free memory before exiting program
  mat_destroy(mat1);
  mat_destroy(mat2);
  return 0;
}

// allocates memory for a matrix with nrows rows and ncols columns
Mat *mat_new(int nrows, int ncols)
{
  Mat *mat = malloc(sizeof(Mat));
  mat->nrows = nrows;
  mat->ncols = ncols;
  mat->rows = vec_new(sizeof(Vec *), nrows);
  for (int i = 0; i < nrows; i++)
  {
    Vec *row = vec_new(sizeof(int), ncols);
    vec_append(mat->rows, &row);
  }
  return mat;
}

// returns the value of the element at row i, column j
int matrix_get(Mat *mat, int i, int j)
{
  Vec *row = *(Vec **)vec_get(mat->rows, i);
  return ((int *)row->data)[j];
}

// sets the value of the element at row i, column j to value
void matrix_set(Mat *mat, int i, int j, int value)
{
  Vec *row = *(Vec **)vec_get(mat->rows, i);
  ((int *)row->data)[j] = value;
}

// reads the elements of the matrix from the user
void mat_input(Mat *mat)
{
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      printf("Enter element at row %d, column %d: ", i, j);
      int value = get_int().value;
      matrix_set(mat, i, j, value);
    }
  }
}

// prints the matrix to the screen
void print_mat(Mat *mat)
{
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      printf("%d ", matrix_get(mat, i, j));
    }
    printf("\n");
  }
}

// returns the product of the two matrices
Mat *mat_mult(Mat *mat1, Mat *mat2)
{
  if (mat1->ncols != mat2->nrows)
  {
    fprintf(stderr, "Error: Incompatible matrix dimensions for multiplication\n");
    return NULL;
  }
  Mat *result = mat_new(mat1->nrows, mat2->ncols);
  if (!result)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return NULL;
  }
  for (int i = 0; i < result->nrows; i++)
  {
    for (int j = 0; j < result->ncols; j++)
    {
      int sum = 0;
      for (int k = 0; k < mat1->ncols; k++)
      {
        sum += matrix_get(mat1, i, k) * matrix_get(mat2, k, j);
      }
      matrix_set(result, i, j, sum);
    }
  }
  return result;
}

// gets an integer from the user
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

// frees memory allocated for the matrix  
void mat_destroy(Mat *mat)
{
  for (int i = 0; i < mat->nrows; i++)
  {
    vec_destroy(*(Vec **)vec_get(mat->rows, i));
  }
  vec_destroy(mat->rows);
  free(mat);
}