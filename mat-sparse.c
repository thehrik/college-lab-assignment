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
} GetStatus;

typedef struct
{
  GetStatus status;
  union
  {
    int *ok;
    const char *err;
  } data;
} ResultGetInt;

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
  int nnz;
  SparseEntry *data;
} SparseMat;

ResultGetInt get_int();
SparseMat *sparse_new(int nrows, int ncols, int max_nnz);
void sparse_add(SparseMat *mat, int row, int col, int value);
void sparse_print(SparseMat *mat);
int sparse_get(SparseMat *mat, int row, int col);
void sparse_mat_destroy(SparseMat *mat);
void mat_print(SparseMat *mat);

int main()
{
  int *val;
  int nrows, ncols;
  printf("enter number of rows: ");
  val = get_int().data.ok;
  if (!val)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return 1;
  }
  nrows = *val;
  printf("enter number of columns: ");
  val = get_int().data.ok;
  if (!val)
  {
    printf("input stopped by user\n");
    return 1;
  }
  ncols = *val;
  if (nrows <= 0 || ncols <= 0)
  {
    fprintf(stderr, "Error: Invalid matrix dimensions\n");
    return 1;
  }
  SparseMat *mat = sparse_new(nrows, ncols, 10);
  if (!mat)
  {
    fprintf(stderr, "Error: Memory allocation failed\n");
    return 1;
  }
  printf("Enter the elements of the matrix:\n");
  for (int i = 0; i < nrows; i++)
  {
    for (int j = 0; j < ncols; j++)
    {
      printf("Enter element at row %d, column %d: ", i, j);
      val = get_int().data.ok;
      if (!val)
      {
        fprintf(stderr, "Error: Memory allocation failed\n");
        sparse_mat_destroy(mat);
        return 1;
      }
      else
      {
        sparse_add(mat, i, j, *val);
      }
    }
  }
  sparse_print(mat);
  printf("Matrix in dense format:\n");
  mat_print(mat);
  sparse_mat_destroy(mat);
  return 0;
}

SparseMat *sparse_new(int nrows, int ncols, int max_nnz)
{
  SparseMat *mat = malloc(sizeof(SparseMat));
  mat->nrows = nrows;
  mat->ncols = ncols;
  mat->nnz = 0;
  mat->data = malloc(sizeof(SparseEntry) * max_nnz);
  return mat;
}

void sparse_add(SparseMat *mat, int row, int col, int value)
{
  if (value == 0)
    return;
  mat->data[mat->nnz].row = row;
  mat->data[mat->nnz].col = col;
  mat->data[mat->nnz].value = value;
  mat->nnz++;
}

void sparse_print(SparseMat *mat)
{
  printf("Sparse matrix (COO format):\n");
  for (size_t i = 0; i < mat->nnz; i++)
  {
    printf("(%d, %d): %d\n", mat->data[i].row, mat->data[i].col, mat->data[i].value);
  }
}

void mat_print(SparseMat *mat)
{
  for (int i = 0; i < mat->nrows; i++)
  {
    for (int j = 0; j < mat->ncols; j++)
    {
      if (sparse_get(mat, i, j) != 0)
      {
        printf("%d ", sparse_get(mat, i, j));
      }
      else
      {
        printf("0 ");
      }
    }
    printf("\n");
  }
}

int sparse_get(SparseMat *mat, int row, int col)
{
  for (size_t i = 0; i < mat->nnz; i++)
  {
    if (mat->data[i].row == row && mat->data[i].col == col)
    {
      return mat->data[i].value;
    }
  }
  return 0;
}

void sparse_mat_destroy(SparseMat *mat)
{
  free(mat->data);
  free(mat);
}

ResultGetInt get_int()
{
  int result;

  while (1)
  {
    String *s = String_new(0);
    if (!s)
    {
      return (ResultGetInt){GET_ERR, .data.err = "memory allocation failed\n"};
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
        return (ResultGetInt){GET_STOPPED, .data.ok = NULL};
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
  return (ResultGetInt){GET_OK, .data.ok = &result};
}