// Ported code, warits.wang@infotm.com

int cloth_get_offset(int t, int h)
{
  int d = t >> 4;
  int rnd = d / h, off = d % h;
  return rnd & 1? h - off: off;
}

#define _sq(x) ((x) * (x))
void cloth_draw(char *buf, int w, int h, int i, int j)
{
  uint32_t r, g, b;
  float s = 3. / (j + 99);
  float y = (j + sin((i * i + _sq(j - 700) * 5) / 100./w) * 35) * s;
  Pixel color;

  r = ((int)((i + w) * s + y) % 2 + (int)((w * 2 - i)
      * s + y) % 2) * 127;
  g = ((int)(5 * ((i + w) * s + y)) % 2
      + (int)(5 * ((w * 2 - i) * s + y)) % 2) * 127;
  b = ((int)(29 * ((i + w) * s + y)) % 2 + (int)(29 *
      ((w * 2 - i) * s + y)) % 2) * 127;

  color.SetRGB(r, g, b, 0xff);
  *((uint32_t *)buf + j * w + i) = color.GetRGBA();
}
