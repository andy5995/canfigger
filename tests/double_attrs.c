#include "test.h"

int
main(void)
{
  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/double_attrs.conf", ',');
  assert(list);

  /* scale = factors, 1.5, 2.0, 0.75 */
  assert(strcmp(list->key, "scale") == 0);
  assert(strcmp(list->value, "factors") == 0);
  double factors[3];
  size_t n = canfigger_get_double_attrs(list, factors, 3);
  assert(n == 3);
  assert(factors[0] == 1.5);
  assert(factors[1] == 2.0);
  assert(factors[2] == 0.75);
  canfigger_free_current_key_node_advance(&list);

  /* point = pos, 10.5, 20.0 — max less than attribute count */
  assert(strcmp(list->key, "point") == 0);
  double pt[1];
  n = canfigger_get_double_attrs(list, pt, 1);
  assert(n == 1);
  assert(pt[0] == 10.5);
  canfigger_free_current_key_node_advance(&list);

  /* noattrs = plain — no attributes, expect 0 */
  assert(strcmp(list->key, "noattrs") == 0);
  double dummy[1];
  n = canfigger_get_double_attrs(list, dummy, 1);
  assert(n == 0);
  canfigger_free_current_key_node_advance(&list);

  assert(list == NULL);
  return 0;
}
