#include "test.h"

int
main(void)
{
  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/parse_color.conf", ',');
  assert(list);

  uint8_t r, g, b, a;

  /* hex_rgb = #FF8000 — hex path, alpha defaults to 255 */
  assert(strcmp(list->key, "hex_rgb") == 0);
  assert(canfigger_parse_color(list, &r, &g, &b, &a) == 3);
  assert(r == 0xFF && g == 0x80 && b == 0x00 && a == 0xFF);
  canfigger_free_current_key_node_advance(&list);

  /* hex_rgba = #FF800080 — hex path with alpha */
  assert(strcmp(list->key, "hex_rgba") == 0);
  assert(canfigger_parse_color(list, &r, &g, &b, &a) == 4);
  assert(r == 0xFF && g == 0x80 && b == 0x00 && a == 0x80);
  canfigger_free_current_key_node_advance(&list);

  /* int_rgb = rgb, 255, 128, 0 — integer path, alpha defaults to 255 */
  assert(strcmp(list->key, "int_rgb") == 0);
  assert(canfigger_parse_color(list, &r, &g, &b, &a) == 3);
  assert(r == 255 && g == 128 && b == 0 && a == 255);
  canfigger_free_current_key_node_advance(&list);

  /* int_rgba = rgba, 255, 128, 0, 128 — integer path with alpha */
  assert(strcmp(list->key, "int_rgba") == 0);
  assert(canfigger_parse_color(list, &r, &g, &b, &a) == 4);
  assert(r == 255 && g == 128 && b == 0 && a == 128);
  canfigger_free_current_key_node_advance(&list);

  /* noattrs = plain — no '#' and no attributes, expect 0 */
  assert(strcmp(list->key, "noattrs") == 0);
  assert(canfigger_parse_color(list, &r, &g, &b, &a) == 0);
  canfigger_free_current_key_node_advance(&list);

  assert(list == NULL);

  /* NULL node */
  assert(canfigger_parse_color(NULL, &r, &g, &b, &a) == 0);

  return 0;
}
