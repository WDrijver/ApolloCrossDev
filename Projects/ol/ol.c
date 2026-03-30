

void main(const short *src, short *dst, short count)
{
  short i;
  for (i = 0; i < count; i++) {
    *dst++ = *--src;
    *dst++ = *--src;
    *dst++ = *--src;
    *dst++ = *--src;
  }
}
