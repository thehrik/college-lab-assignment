#ifndef RESULT_H
#define RESULT_H

typedef enum {
  RESULT_OK,
  RESULT_ERR
} ResultStatus;

typedef struct {
  ResultStatus status;
  int value;      // The parsed integer (valid if status == RESULT_OK)
  const char *error; // Error message (valid if status == RESULT_ERR)
} ResultInt;

typedef struct {
  ResultStatus status;   // The parsed string (valid if status == RESULT_OK)
  const char *error; // Error message (valid if status == RESULT_ERR)
} ResultString;

typedef struct {
  ResultStatus status;   // The parsed vector (valid if status == RESULT_OK)
  const char *error; // Error message (valid if status == RESULT_ERR)
} ResultVecInt;

#endif // RESULT_H