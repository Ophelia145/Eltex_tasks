int
find_quantity_of_1 (int num)
{
  int count = 0;
  unsigned int offset = 1;
  for (int i = 0; i < 32; i++)
    {
      if (num & (offset << i))
        {
          count++;
        }
    }
  return count;
}