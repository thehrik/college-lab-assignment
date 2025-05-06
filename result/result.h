#ifndef RESULT_H
#define RESULT_H

#include "../types/types.h" // for String

typedef enum
{
  RESULT_OK,
  RESULT_ERR
} ResultStatus;

typedef struct
{
  ResultStatus status;
  union
  {
    int ok;          // (valid if status == RESULT_OK)
    const char *err; // Error message (valid if status == RESULT_ERR)
  } data;
} ResultInt;

typedef struct
{
  ResultStatus status;
  union
  {
    String *ok;      // (valid if status == RESULT_OK)
    const char *err; // Error message (valid if status == RESULT_ERR)
  } data;
} ResultString;

typedef struct
{
  ResultStatus status;
  union
  {
    Vec *ok;      // (valid if status == RESULT_OK)
    const char *err; // Error message (valid if status == RESULT_ERR)
  } data;
} ResultVecInt;

#endif // RESULT_H