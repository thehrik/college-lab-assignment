#ifndef RESULT_H
#define RESULT_H

#include "../types/types.h" // for String

typedef enum
{
  OK,
  ERR
} Status;

typedef struct
{
  Status status;
  union
  {
    void *ok;        // (valid if status == OK)
    const char *err_str; // Error message (valid if status == RESULT_ERR)
  } data;
} Result;

typedef enum
{
  READ_OK,
  READ_ERR,
  READ_STOPPED,
} ReadStatus;

typedef struct
{
  ReadStatus status;
  union
  {
    void *ok;
    const char *err_str;
  } data;
} ReadResult;

int destroy_result(Result *r);
int destroy_read_result(ReadResult *r);

typedef struct
{
  Status status;
  union
  {
    String *ok;      // (valid if status == OK)
    const char *err_str; // Error message (valid if status == RESULT_ERR)
  } data;
} ResultString;

typedef struct
{
  Status status;
  union
  {
    Vec *ok;         // (valid if status == OK)
    const char *err_str; // Error message (valid if status == RESULT_ERR)
  } data;
} ResultVecInt;

#endif // RESULT_H