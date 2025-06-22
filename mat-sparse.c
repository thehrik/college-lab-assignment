#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h" // Assuming String_new, String_read_line, String_destroy, ResultString, ERR
#include "./result/result.h" // Assuming Result, ERR, OK
#include "./input/input.h"   // Assuming int_read_line, destroy_read_result, ReadResult, READ_ERR, READ_OK, READ_STOPPED

typedef struct
{
  size_t row;
  size_t col;
  int value;
} SparseEntry;

typedef struct
{
  int nrows;
  int ncols;
  int nnz;           // Number of non-zero elements
  size_t capacity;   // Current allocated capacity for data
  SparseEntry *data; // Array of non-zero entries
} SparseMat;

// Function prototypes
SparseMat *sparse_new(int nrows, int ncols); // Removed max_nnz, will dynamically resize
void sparse_add(SparseMat *mat, int row, int col, int value);
void sparse_print(SparseMat *mat);
int sparse_get(SparseMat *mat, int row, int col);
void sparse_mat_destroy(SparseMat *mat);
void mat_print(SparseMat *mat);

// Helper function for safe integer input
int get_matrix_dimension_input(const char *prompt_text);
int get_matrix_element_input(const char *prompt_text);

// --- Main Function ---
int main()
{
  int nrows, ncols;
  SparseMat *mat = NULL;

  printf("--- Sparse Matrix Creation ---\n");

  // Get matrix dimensions
  nrows = get_matrix_dimension_input("Enter number of rows: ");
  if (nrows == 0)
    return 1; // Input stopped or error occurred
  ncols = get_matrix_dimension_input("Enter number of columns: ");
  if (ncols == 0)
    return 1; // Input stopped or error occurred

  // Allocate initial sparse matrix with a default capacity
  // We'll grow this capacity dynamically if needed.
  mat = sparse_new(nrows, ncols);
  if (!mat)
  {
    fprintf(stderr, "Error: Memory allocation failed for sparse matrix.\n");
    return 1;
  }

  printf("\nEnter the elements of the matrix (non-zero values will be stored):\n");
  for (int i = 0; i < nrows; i++)
  {
    for (int j = 0; j < ncols; j++)
    {
      char prompt[100];
      snprintf(prompt, sizeof(prompt), "Enter element at row %d, column %d: ", i + 1, j + 1);
      int value = get_matrix_element_input(prompt);

      if (value == -1) // -1 indicates input stopped by user
      {
        fprintf(stderr, "Input stopped by user. Cleaning up and exiting.\n");
        sparse_mat_destroy(mat);
        return 1;
      }
      // Only add non-zero values to the sparse representation
      if (value != 0)
      {
        sparse_add(mat, i, j, value);
      }
    }
  }

  printf("\n--- Sparse Matrix Representation ---\n");
  sparse_print(mat);

  printf("\n--- Matrix in Dense Format ---\n");
  mat_print(mat);

  printf("\nCleaning up memory...\n");
  sparse_mat_destroy(mat);
  printf("Program finished successfully.\n");
  return 0;
}

// --- Helper Functions for Input ---

// Helper function to get matrix dimensions (must be positive)
// Returns the valid positive integer, or 0 if an error occurred or input was stopped.
int get_matrix_dimension_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s (enter 'q' to quit): ", prompt_text);
    ri = int_read_line();
    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid positive integer.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      fprintf(stderr, "Input stopped by user.\n");
      destroy_read_result(&ri);
      return 0; // Signal user stop
    }
    else // READ_OK
    {
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      if (value <= 0)
      { // Dimensions must be strictly positive
        fprintf(stderr, "Error: Dimensions must be positive integers (>= 1). Please try again.\n");
        continue;
      }
      return value;
    }
  }
}

// Helper function to get matrix element value
// Returns the integer value, or -1 if input was stopped by user.
int get_matrix_element_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s (enter 'q' to cancel): ", prompt_text);
    ri = int_read_line();
    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      destroy_read_result(&ri);
      return -1; // Signal user stop with a distinct value
    }
    else // READ_OK
    {
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      return value; // Return the valid integer (can be 0)
    }
  }
}

// --- Sparse Matrix Operations Implementation ---

#define INITIAL_SPARSE_CAPACITY 10
#define RESIZE_FACTOR 2

// Allocates memory for a new sparse matrix
SparseMat *sparse_new(int nrows, int ncols)
{
  SparseMat *mat = malloc(sizeof(SparseMat));
  if (!mat)
  {
    fprintf(stderr, "Memory allocation failed for SparseMat struct.\n");
    return NULL;
  }

  mat->nrows = nrows;
  mat->ncols = ncols;
  mat->nnz = 0;
  mat->capacity = INITIAL_SPARSE_CAPACITY; // Start with a default capacity

  mat->data = malloc(sizeof(SparseEntry) * mat->capacity);
  if (!mat->data)
  {
    fprintf(stderr, "Memory allocation failed for SparseEntry data array.\n");
    free(mat); // Free the partially allocated struct
    return NULL;
  }
  return mat;
}

// Adds a non-zero element to the sparse matrix. Handles dynamic resizing.
void sparse_add(SparseMat *mat, int row, int col, int value)
{
  if (value == 0) // We only store non-zero values
    return;

  // Check if we need to resize the data array
  if (mat->nnz >= mat->capacity)
  {
    size_t new_capacity = mat->capacity * RESIZE_FACTOR;
    SparseEntry *new_data = realloc(mat->data, sizeof(SparseEntry) * new_capacity);
    if (!new_data)
    {
      fprintf(stderr, "Error: Failed to reallocate memory for sparse matrix data. Cannot add more elements.\n");
      // In a real application, you might want to return an error status here.
      // For now, we'll just stop adding new elements to prevent crash.
      return;
    }
    mat->data = new_data;
    mat->capacity = new_capacity;
    printf("Sparse matrix data array resized to %zu elements.\n", mat->capacity);
  }

  // Add the new entry
  mat->data[mat->nnz].row = row;
  mat->data[mat->nnz].col = col;
  mat->data[mat->nnz].value = value;
  mat->nnz++;
}

// Prints the sparse matrix in COO format (row, col, value)
void sparse_print(SparseMat *mat)
{
  if (mat == NULL)
  {
    printf("Cannot print a NULL sparse matrix.\n");
    return;
  }
  printf("Sparse matrix (COO format) - %d non-zero elements:\n", mat->nnz);
  if (mat->nnz == 0)
  {
    printf("No non-zero elements.\n");
    return;
  }
  for (size_t i = 0; i < mat->nnz; i++)
  {
    printf("(Row: %zu, Col: %zu, Value: %d)\n", mat->data[i].row + 1, mat->data[i].col + 1, mat->data[i].value); // 1-based indexing for user
  }
}

// Retrieves the value at a specific row and column. Returns 0 if not found.
int sparse_get(SparseMat *mat, int row, int col)
{
  if (mat == NULL)
  {
    fprintf(stderr, "Error: Cannot get element from a NULL sparse matrix.\n");
    return 0;
  }
  // Basic bounds checking (though `sparse_add` should enforce this for stored entries)
  if (row < 0 || row >= mat->nrows || col < 0 || col >= mat->ncols)
  {
    fprintf(stderr, "Warning: Attempted to get element at out-of-bounds position (%d, %d).\n", row, col);
    return 0;
  }
  for (size_t i = 0; i < mat->nnz; i++)
  {
    if (mat->data[i].row == (size_t)row && mat->data[i].col == (size_t)col)
    {
      return mat->data[i].value;
    }
  }
  return 0; // Return 0 if the element is not explicitly stored (meaning it's a zero)
}

// Prints the sparse matrix in dense (full) format
void mat_print(SparseMat *mat)
{
  if (mat == NULL)
  {
    printf("Cannot print a NULL matrix in dense format.\n");
    return;
  }
  printf("Matrix in Dense Format (%dx%d):\n", mat->nrows, mat->ncols);
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      printf("%d\t", sparse_get(mat, i, j)); // Use tab for better spacing
    }
    printf("\n");
  }
}

// Frees all memory allocated for the sparse matrix
void sparse_mat_destroy(SparseMat *mat)
{
  if (mat == NULL)
  {
    return; // Nothing to destroy
  }
  if (mat->data != NULL)
  {
    free(mat->data); // Free the array of SparseEntry structs
    mat->data = NULL;
  }
  free(mat); // Free the SparseMat struct itself
}