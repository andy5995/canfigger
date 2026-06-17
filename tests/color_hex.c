#include "test.h"

int
main(void)
{
  uint8_t r, g, b, a;

  /* #RRGGBB — alpha defaults to 255 */
  assert(canfigger_parse_color_hex("#FF8000", &r, &g, &b, &a) == 3);
  assert(r == 0xFF && g == 0x80 && b == 0x00 && a == 0xFF);

  /* #RRGGBBAA */
  assert(canfigger_parse_color_hex("#FF800080", &r, &g, &b, &a) == 4);
  assert(r == 0xFF && g == 0x80 && b == 0x00 && a == 0x80);

  /* lowercase */
  assert(canfigger_parse_color_hex("#ff8000", &r, &g, &b, &a) == 3);
  assert(r == 0xFF && g == 0x80 && b == 0x00 && a == 0xFF);

  /* black and white */
  assert(canfigger_parse_color_hex("#000000", &r, &g, &b, &a) == 3);
  assert(r == 0 && g == 0 && b == 0 && a == 0xFF);
  assert(canfigger_parse_color_hex("#FFFFFF", &r, &g, &b, &a) == 3);
  assert(r == 0xFF && g == 0xFF && b == 0xFF && a == 0xFF);

  /* fully transparent */
  assert(canfigger_parse_color_hex("#00000000", &r, &g, &b, &a) == 4);
  assert(r == 0 && g == 0 && b == 0 && a == 0);

  /* NULL input */
  assert(canfigger_parse_color_hex(NULL, &r, &g, &b, &a) == 0);

  /* missing '#' */
  assert(canfigger_parse_color_hex("FF8000", &r, &g, &b, &a) == 0);

  /* wrong length */
  assert(canfigger_parse_color_hex("#FF80", &r, &g, &b, &a) == 0);
  assert(canfigger_parse_color_hex("#FF80000", &r, &g, &b, &a) == 0);

  return 0;
}
