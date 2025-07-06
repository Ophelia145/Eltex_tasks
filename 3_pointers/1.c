#include "main.h"

void
change_3rd_byte (int *n, unsigned char new_byte)
{
  unsigned char *byte_ptr = (unsigned char *)n;
  byte_ptr[2] = new_byte;
}