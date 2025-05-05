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
    int value; // The parsed integer (valid if status == RESULT_OK)
    const char *err;
  } data; // Error message (valid if status == RESULT_ERR)
} ResultInt;

typedef struct
{
  ResultStatus status; // The parsed string (valid if status == RESULT_OK)
  union
  {
    String *ok; // The parsed integer (valid if status == RESULT_OK)
    const char *err;
  } data; // Error message (valid if status == RESULT_ERR)
} ResultString;

typedef struct
{
  ResultStatus status; // The parsed vector (valid if status == RESULT_OK)
  union
  {
    VecInt *ok; // The parsed integer (valid if status == RESULT_OK)
    const char *err;
  } data; // Error message (valid if status == RESULT_ERR)
} ResultVecInt;

#endif // RESULT_H