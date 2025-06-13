#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h" // Assuming String_new, String_read_line, String_destroy, ResultString, ERR
#include "./result/result.h" // Assuming Result, ERR, OK
#include "./vector/vector.h" // Assuming Vec, vec_new, vec_append, vec_get, vec_destroy
#include "./input/input.h"   // Assuming int_read_line, destroy_read_result, ReadResult, READ_ERR, READ_OK, READ_STOPPED
#include "./types/types.h"   // Assuming common type definitions if any are used by the above

typedef struct
{
  Vec *rows;
  int nrows;
  int ncols;
} Mat;

// --- Function Prototypes ---
Mat *mat_new(int rows, int cols);
ReadResult mat_input(Mat *mat);
void print_mat(Mat *mat);
int matrix_get(Mat *mat, int i, int j);
void matrix_set(Mat *mat, int i, int j, int value);
Mat *mat_mult(Mat *mat1, Mat *mat2);
void mat_destroy(Mat *mat);

// Helper function to get an integer input with error handling
// Returns the valid integer (must be positive), or 0 if an error occurred or input was stopped.
int get_dimension_input(const char *prompt_text);

// --- Main Function ---
int main()
{
  int rows1, cols1;
  int rows2, cols2;
  Mat *mat1 = NULL;
  Mat *mat2 = NULL;
  Mat *result_mat = NULL;

  printf("--- Matrix Multiplication Program ---\n");

  // --- Input for the first matrix dimensions ---
  printf("\n--- First Matrix Dimensions ---\n");
  rows1 = get_dimension_input("Enter number of rows for matrix 1: ");
  if (rows1 == 0)
    return 1; // get_dimension_input returns 0 on error/stop
  cols1 = get_dimension_input("Enter number of columns for matrix 1: ");
  if (cols1 == 0)
    return 1;

  mat1 = mat_new(rows1, cols1);
  if (!mat1)
  {
    fprintf(stderr, "Error: Memory allocation failed for matrix 1.\n");
    return 1;
  }

  printf("Enter elements for the first matrix:\n");
  ReadResult rm1 = mat_input(mat1);
  if (rm1.status != READ_OK)
  {
    fprintf(stderr, "Error during input for matrix 1.\n");
    destroy_read_result(&rm1); // Clean up result if not OK
    mat_destroy(mat1);
    return 1;
  }
  destroy_read_result(&rm1); // Destroy result if OK

  // --- Input for the second matrix dimensions ---
  printf("\n--- Second Matrix Dimensions ---\n");
  rows2 = get_dimension_input("Enter number of rows for matrix 2: ");
  if (rows2 == 0)
  {
    mat_destroy(mat1); // Clean up mat1 before exiting
    return 1;
  }
  cols2 = get_dimension_input("Enter number of columns for matrix 2: ");
  if (cols2 == 0)
  {
    mat_destroy(mat1); // Clean up mat1 before exiting
    return 1;
  }

  mat2 = mat_new(rows2, cols2);
  if (!mat2)
  {
    fprintf(stderr, "Error: Memory allocation failed for matrix 2.\n");
    mat_destroy(mat1); // Clean up mat1
    return 1;
  }

  printf("Enter elements for the second matrix:\n");
  ReadResult rm2 = mat_input(mat2);
  if (rm2.status != READ_OK)
  {
    fprintf(stderr, "Error during input for matrix 2.\n");
    destroy_read_result(&rm2); // Clean up result if not OK
    mat_destroy(mat1);
    mat_destroy(mat2);
    return 1;
  }
  destroy_read_result(&rm2); // Destroy result if OK

  // --- Perform matrix multiplication ---
  printf("\nPerforming Matrix Multiplication...\n");
  result_mat = mat_mult(mat1, mat2);
  if (!result_mat)
  {
    fprintf(stderr, "Error: Matrix multiplication failed (incompatible dimensions or memory allocation issue).\n");
    // Specific error message for dimension mismatch already printed by mat_mult
    mat_destroy(mat1);
    mat_destroy(mat2);
    return 1;
  }

  printf("\n--- Result Matrix (Multiplication) ---\n");
  print_mat(result_mat);

  // --- Clean up memory ---
  printf("\nCleaning up memory...\n");
  mat_destroy(result_mat);
  mat_destroy(mat1);
  mat_destroy(mat2);

  printf("Program finished successfully.\n");
  return 0;
}

// --- Helper Function to get Integer Dimensions ---
// Returns the valid positive integer, or 0 if an error occurred or input was stopped.
int get_dimension_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s", prompt_text);
    ri = int_read_line();
    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Error: %s. Please try again.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      fprintf(stderr, "Input stopped by user. Exiting.\n");
      destroy_read_result(&ri);
      return 0; // Indicate stopping to main
    }
    else // READ_OK
    {
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri); // Free allocated data
      if (value < 1)
      { // Dimensions must be positive integers
        fprintf(stderr, "Error: Dimensions must be positive integers (>= 1). Please try again.\n");
        continue;
      }
      return value;
    }
  }
}

// --- Matrix Utility Functions ---

// allocates memory for a matrix with nrows rows and ncols columns
Mat *mat_new(int nrows, int ncols)
{
  Mat *mat = malloc(sizeof(Mat));
  if (!mat)
  {
    fprintf(stderr, "Memory allocation failed for Mat struct.\n");
    return NULL;
  }
  mat->nrows = nrows;
  mat->ncols = ncols;

  mat->rows = vec_new(sizeof(Vec *), nrows);
  if (!mat->rows)
  {
    fprintf(stderr, "Memory allocation failed for rows vector.\n");
    free(mat);
    return NULL;
  }

  // Allocate each row vector
  for (int i = 0; i < nrows; i++)
  {
    Vec *row = vec_new(sizeof(int), ncols);
    if (!row)
    {
      fprintf(stderr, "Memory allocation failed for row %d.\n", i);
      // Clean up already allocated rows and the main vector before returning NULL
      for (int k = 0; k < i; k++)
      {
        vec_destroy(*(Vec **)vec_get(mat->rows, k));
      }
      vec_destroy(mat->rows); // Destroy the vector storing row pointers
      free(mat);              // Free the Mat struct itself
      return NULL;
    }
    vec_append(mat->rows, &row); // Append pointer to the new row vector
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
ReadResult mat_input(Mat *mat)
{
  ReadResult value_result;
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      while (1)
      {
        printf("Enter element at row %d, column %d: ", i + 1, j + 1); // User-friendly 1-based indexing
        value_result = int_read_line();
        if (value_result.status == READ_ERR)
        {
          fprintf(stderr, "Error: %s. Please try again.\n", value_result.data.err_str);
          destroy_read_result(&value_result);
          continue;
        }
        else if (value_result.status == READ_STOPPED)
        {
          fprintf(stderr, "Input stopped by user.\n");
          // Important: Return READ_STOPPED so main can clean up
          return (ReadResult){READ_STOPPED, .data.ok = NULL};
        }
        matrix_set(mat, i, j, *(int *)value_result.data.ok);
        destroy_read_result(&value_result); // Destroy the result after use
        break;
      }
    }
  }
  return (ReadResult){READ_OK, .data.ok = NULL}; // Indicate success
}

// prints the matrix to the screen
void print_mat(Mat *mat)
{
  if (mat == NULL)
  { // Handle case where matrix is NULL
    printf("Cannot print a NULL matrix.\n");
    return;
  }
  printf("Matrix (%dx%d):\n", mat->nrows, mat->ncols);
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      printf("%d\t", matrix_get(mat, i, j)); // Use tab for better spacing
    }
    printf("\n");
  }
}

// returns the product of the two matrices
Mat *mat_mult(Mat *mat1, Mat *mat2)
{
  if (mat1 == NULL || mat2 == NULL)
  {
    fprintf(stderr, "Error: Cannot multiply a NULL matrix.\n");
    return NULL;
  }
  if (mat1->ncols != mat2->nrows)
  {
    fprintf(stderr, "Error: Incompatible matrix dimensions for multiplication. "
                    "Number of columns in first matrix (%d) must equal number of rows in second matrix (%d).\n",
            mat1->ncols, mat2->nrows);
    return NULL;
  }

  Mat *result = mat_new(mat1->nrows, mat2->ncols);
  if (!result)
  {
    fprintf(stderr, "Error: Memory allocation failed for result matrix in multiplication.\n");
    return NULL;
  }

  for (int i = 0; i < result->nrows; i++)
  {
    for (int j = 0; j < result->ncols; j++)
    {
      int sum = 0;
      for (int k = 0; k < mat1->ncols; k++) // mat1->ncols is same as mat2->nrows
      {
        sum += matrix_get(mat1, i, k) * matrix_get(mat2, k, j);
      }
      matrix_set(result, i, j, sum);
    }
  }
  return result;
}

// frees memory allocated for the matrix
void mat_destroy(Mat *mat)
{
  if (mat == NULL)
  {
    return; // Nothing to destroy if matrix pointer is NULL
  }
  if (mat->rows != NULL)
  {
    // Destroy each row vector pointed to by the 'rows' vector
    for (int i = 0; i < mat->nrows; i++)
    {
      Vec *row = *(Vec **)vec_get(mat->rows, i);
      if (row != NULL) // Defensive check, though vec_new should ensure non-NULL
      {
        vec_destroy(row);
      }
    }
    // Destroy the vector that holds pointers to the rows
    vec_destroy(mat->rows);
  }
  // Finally, free the Mat struct itself
  free(mat);
}