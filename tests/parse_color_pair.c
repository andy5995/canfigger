#include "test.h"

int
main(void)
{
  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/parse_color_pair.conf", ',');
  assert(list);

  uint8_t r1, g1, b1, a1, r2, g2, b2, a2;

  /* button = pair, #FFFF00, #000000 — both RGB, alpha defaults to 255 */
  assert(strcmp(list->key, "button") == 0);
  assert(canfigger_parse_color_pair(list, &r1, &g1, &b1, &a1,
                                    &r2, &g2, &b2, &a2) == 2);
  assert(r1 == 0xFF && g1 == 0xFF && b1 == 0x00 && a1 == 0xFF);
  assert(r2 == 0x00 && g2 == 0x00 && b2 == 0x00 && a2 == 0xFF);
  canfigger_free_current_key_node_advance(&list);

  /* highlight = pair, #FF800080, #00000080 — both RGBA */
  assert(strcmp(list->key, "highlight") == 0);
  assert(canfigger_parse_color_pair(list, &r1, &g1, &b1, &a1,
                                    &r2, &g2, &b2, &a2) == 2);
  assert(r1 == 0xFF && g1 == 0x80 && b1 == 0x00 && a1 == 0x80);
  assert(r2 == 0x00 && g2 == 0x00 && b2 == 0x00 && a2 == 0x80);
  canfigger_free_current_key_node_advance(&list);

  /* solo = pair, #FFFFFF — only one color, returns 1 */
  assert(strcmp(list->key, "solo") == 0);
  assert(canfigger_parse_color_pair(list, &r1, &g1, &b1, &a1,
                                    &r2, &g2, &b2, &a2) == 1);
  assert(r1 == 0xFF && g1 == 0xFF && b1 == 0xFF && a1 == 0xFF);
  canfigger_free_current_key_node_advance(&list);

  /* invalid = pair, notacolor, #000000 — first attr not hex, returns 0 */
  assert(strcmp(list->key, "invalid") == 0);
  assert(canfigger_parse_color_pair(list, &r1, &g1, &b1, &a1,
                                    &r2, &g2, &b2, &a2) == 0);
  canfigger_free_current_key_node_advance(&list);

  assert(list == NULL);

  /* NULL node */
  assert(canfigger_parse_color_pair(NULL, &r1, &g1, &b1, &a1,
                                    &r2, &g2, &b2, &a2) == 0);

  return 0;
}
