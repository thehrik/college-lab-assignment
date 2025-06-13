#include <stdlib.h>
#include "result.h"

int destroy_result(Result *r)
{
  if (r == NULL)
  {
    return -1;
  }
  if (r->status == OK)
  {
    free(r->data.ok);
    r->data.ok = NULL;
    return 0;
  }
  return 1;
}

int destroy_read_result(ReadResult *r){
  if (r == NULL)
  {
    return -1;
  }
  if (r->status == READ_OK)
  {
    free(r->data.ok);
    r->data.ok = NULL;
    return 0;
  }
  return 1;
}