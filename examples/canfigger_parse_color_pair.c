#include <stdint.h>
#include <stdio.h>
#include "canfigger.h"

int
main(void)
{
  //! [canfigger_parse_color_pair]
  /*
   * Config line: button = pair, #FFFF00, #000000
   * First attribute is foreground, second is background.
   */
  struct Canfigger *list = canfigger_parse_file("colors.conf", ',');
  while (list)
  {
    uint8_t r1, g1, b1, a1, r2, g2, b2, a2;
    if (canfigger_parse_color_pair(list, &r1, &g1, &b1, &a1,
                                   &r2, &g2, &b2, &a2) == 2)
      fprintf(stderr, "%s: fg=rgba(%u,%u,%u,%u) bg=rgba(%u,%u,%u,%u)\n",
              list->key, r1, g1, b1, a1, r2, g2, b2, a2);
    canfigger_free_current_key_node_advance(&list);
  }
  //! [canfigger_parse_color_pair]
  return 0;
}
