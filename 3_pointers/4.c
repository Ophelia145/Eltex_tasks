#include "main.h"
#include <string.h>
char *
strstrmy (char *str, char *ministr)
{
  void *found = NULL;
  if (*ministr == '\0')
    {
      found = (char *)str;
    }
  while (*str != '\0')
    {
      char *s = str;
      char *m = ministr;
      int count = 0;
      while ((*s != '\0' && *m != '\0') && *s == *m)
        {
          s++;
          m++;
          count++;
        }

      if (*m == '\0')
        {
          found = s - count;
          break;
        }
      str++;
    }
  return found;
}