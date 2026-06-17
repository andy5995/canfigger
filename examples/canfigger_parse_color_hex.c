#include <stdint.h>
#include <stdio.h>
#include "canfigger.h"

int
main(void)
{
  //! [canfigger_parse_color_hex]
  uint8_t r, g, b, a;

  if (canfigger_parse_color_hex("#FF8000", &r, &g, &b, &a) == 3)
    fprintf(stderr, "rgb: %u %u %u\n", r, g, b);

  if (canfigger_parse_color_hex("#FF800080", &r, &g, &b, &a) == 4)
    fprintf(stderr, "rgba: %u %u %u %u\n", r, g, b, a);
  //! [canfigger_parse_color_hex]
  return 0;
}
