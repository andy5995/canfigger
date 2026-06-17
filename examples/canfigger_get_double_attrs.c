/*
 * Compilable example for canfigger_get_double_attrs().
 * Config line: scale = factors, 1.0, 0.75, 2.5
 */
#include <stdio.h>
#include <string.h>
#include "canfigger.h"

static void
example(struct Canfigger *node)
{
//! [canfigger_get_double_attrs]
  /* Check the value tag, then validate the count before using the array. */
  if (node->value && strcmp(node->value, "factors") == 0)
  {
    double v[3];
    if (canfigger_get_double_attrs(node, v, 3) == 3)
    {
      fprintf(stderr, "factors: %g %g %g\n", v[0], v[1], v[2]);
    }
  }
//! [canfigger_get_double_attrs]
}

int
main(void)
{
  struct Canfigger *list = canfigger_parse_file("scale.conf", ',');
  if (list)
  {
    example(list);
    canfigger_free_list(&list);
  }
  return 0;
}
