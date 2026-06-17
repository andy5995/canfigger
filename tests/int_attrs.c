#include "test.h"

int
main(void)
{
  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/int_attrs.conf", ',');
  assert(list);

  /* board = rect, 10, 20, 50, 50 */
  assert(strcmp(list->key, "board") == 0);
  assert(strcmp(list->value, "rect") == 0);
  int coords[4];
  size_t n = canfigger_get_int_attrs(list, coords, 4);
  assert(n == 4);
  assert(coords[0] == 10);
  assert(coords[1] == 20);
  assert(coords[2] == 50);
  assert(coords[3] == 50);
  canfigger_free_current_key_node_advance(&list);

  /* seat = pos, 100, 200 */
  assert(strcmp(list->key, "seat") == 0);
  int point[2];
  n = canfigger_get_int_attrs(list, point, 2);
  assert(n == 2);
  assert(point[0] == 100);
  assert(point[1] == 200);
  canfigger_free_current_key_node_advance(&list);

  /* negative = offset, -10, -20 */
  assert(strcmp(list->key, "negative") == 0);
  int neg[2];
  n = canfigger_get_int_attrs(list, neg, 2);
  assert(n == 2);
  assert(neg[0] == -10);
  assert(neg[1] == -20);
  canfigger_free_current_key_node_advance(&list);

  /* many = list, 1, 2, 3, 4, 5, 6 — max less than attribute count */
  assert(strcmp(list->key, "many") == 0);
  int few[3];
  n = canfigger_get_int_attrs(list, few, 3);
  assert(n == 3);
  assert(few[0] == 1);
  assert(few[1] == 2);
  assert(few[2] == 3);
  canfigger_free_current_key_node_advance(&list);

  /* noattrs = plain — no attributes, expect 0 */
  assert(strcmp(list->key, "noattrs") == 0);
  int dummy[1];
  n = canfigger_get_int_attrs(list, dummy, 1);
  assert(n == 0);
  canfigger_free_current_key_node_advance(&list);

  assert(list == NULL);
  return 0;
}
