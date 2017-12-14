// Ported code, warits.wang@infotm.com

int water_get_offset(int t, int h)
{
  return (int)(100 * h * sin((double)t / 100000) + 100 * h) % h;
}

static inline int __water_r(int n) {
  return rand() % n;
}

static int __water_rand_draw(char *buf, int w, int h,
          int i, int j, int cs)
{
  uint32_t *cp = (uint32_t *)buf + j * w + i;
  int c, ni = (i + __water_r(3) - 1 + w) % w,
      nj = (j + __water_r(4)) % h;

  if(j >= h) {
    *cp = *((uint32_t *)buf + (j % h) * w + i);
    return 0;
  }

  c = (*cp >> cs) & 0xff;
  c = !c? (!__water_r(999)? __water_r(256):
    __water_rand_draw(buf, w, h, ni, nj, cs)): c;

  *cp |= c << cs;
  return c;
}

void water_draw(char *buf, int w, int h, int i, int j)
{
  __water_rand_draw(buf, w, h, i, j, 16);
  __water_rand_draw(buf, w, h, i, j, 8);
  __water_rand_draw(buf, w, h, i, j, 0);
}
