#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define n 50
#define out_of 100
typedef struct
{
  char name[10];
  char second_name[10];
  char tel[10];
} abonent;

void start (abonent *array);
int add (abonent *array);
int delete (abonent *array);
int search_by_name (abonent *array);
int print_all_the_records (abonent *array);
