/*
 * Compilable example for canfigger_get_int_attrs().
 * Config line: board = rect, 10, 20, 640, 480
 */
#include <stdio.h>
#include <string.h>
#include "canfigger.h"

static void
example(struct Canfigger *node)
{
//! [canfigger_get_int_attrs]
  /* Check the value tag, then validate the count before using the array. */
  if (node->value && strcmp(node->value, "rect") == 0)
  {
    int v[4];
    if (canfigger_get_int_attrs(node, v, 4) == 4)
    {
      int x = v[0], y = v[1], w = v[2], h = v[3];
      fprintf(stderr, "rect: x=%d y=%d w=%d h=%d\n", x, y, w, h);
    }
  }
//! [canfigger_get_int_attrs]
}

int
main(void)
{
  struct Canfigger *list = canfigger_parse_file("board.conf", ',');
  if (list)
  {
    example(list);
    canfigger_free_list(&list);
  }
  return 0;
}
