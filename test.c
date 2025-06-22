#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "./string/string.h"
#include "./result/result.h"
#include "./parser/parser.h"
#include "./vector/vector.h"
#include "./input/input.h"

void main()
{
  void* a = malloc(sizeof(u_int8_t));
  void* b = malloc(sizeof(u_int8_t));
  *(u_int16_t*)a = 1000;
  *(u_int8_t*) b = 100;
  
  printf("%d \n", *(u_int8_t*)a);
  printf("%d \n", a);
  printf("%d \n", *(u_int8_t*)++a);
  printf("%d \n", b);
  printf("%d \n", *(u_int8_t*)(b));
  return;
}