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

typedef struct node
{
  struct node *next;
  struct node *prev;
  abonent person;
} node;

typedef struct
{
  node *head;
  node *tail;
  size_t size;
} double_list;

void init (double_list *journal);

void clear_input_buffer (void);
void start (double_list *journal);
void push_back (double_list *journal);
abonent new_abonent (void);
int delete (double_list *journal);
int search_by_name (double_list *journal);
int print_all_the_records (double_list *journal);
void free_list (double_list *journal);