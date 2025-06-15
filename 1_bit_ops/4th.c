int
change (int num, int unum)
{
  num &= ~(255 << 16);
  num |= (unum << 16);
  return num;
}
