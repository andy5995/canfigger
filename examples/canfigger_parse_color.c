#include <stdint.h>
#include <stdio.h>
#include "canfigger.h"

int
main(void)
{
  //! [canfigger_parse_color]
  /*
   * Config file entries this example handles:
   *   bg_hex  = #FF8000
   *   bg_rgb  = rgb, 255, 128, 0
   *   bg_rgba = rgba, 255, 128, 0, 128
   */
  struct Canfigger *list = canfigger_parse_file("colors.conf", ',');
  while (list)
  {
    uint8_t r, g, b, a;
    int n = canfigger_parse_color(list, &r, &g, &b, &a);
    if (n >= 3)
      fprintf(stderr, "%s: rgba(%u,%u,%u,%u)\n", list->key, r, g, b, a);
    canfigger_free_current_key_node_advance(&list);
  }
  //! [canfigger_parse_color]
  return 0;
}
