#ifndef CALC_H
#define CALC_H
#include <stdio.h>
#include <stdlib.h>

__attribute__ ((visibility ("default"))) void add (int a, int b);
__attribute__ ((visibility ("default"))) void sub (int a, int b);
__attribute__ ((visibility ("default"))) void mul (int a, int b);
__attribute__ ((visibility ("default"))) void divi (int a, int b);

#endif