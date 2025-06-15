#include "main.h"
int
find_1st_1_in_num (int num)
{
  int start = 0;
  unsigned int offset = 1;
  for (int i = 32 - 1; i >= 0; i--)
    {
      if (num & (offset << i))
        {
          start = i;
          break;
        }
    }
  return start;
}

void
print_bin_wth_0_at_the_beginning (int num, int start)
{
  unsigned int offset = 1;
  for (int i = start; i >= 0; i--)
    {
      putchar ((num & (offset << i)) ? '1' : '0');
    }
  putchar ('\n');
}
