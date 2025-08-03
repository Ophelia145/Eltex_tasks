#include "main.h"
void
format_size (off_t size, wchar_t *buffer, size_t buf_size)
{
  const wchar_t *units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
  int unit = 0;
  double size_d = size;

  while (size_d >= 1024 && unit < 4)
    {
      size_d /= 1024;
      unit++;
    }

  if (unit == 0 || size_d == (int)size_d)
    {
      swprintf (buffer, buf_size, L"%d %ls", (int)size_d, units[unit]);
    }
  else
    {
      swprintf (buffer, buf_size, L"%.1f %ls", size_d, units[unit]);
    }
}