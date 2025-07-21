#ifndef CALC_H
#define CALC_H
#include <stdio.h>
#include <stdlib.h>

// объяв видимости для C и C++
#if defined(__cplusplus)
#define EXPORT extern "C" __attribute__ ((visibility ("default")))
#else
#define EXPORT __attribute__ ((visibility ("default")))
#endif

EXPORT void add (int a, int b);
EXPORT void sub (int a, int b);
EXPORT void mul (int a, int b);
EXPORT void divi (int a, int b);

#endif