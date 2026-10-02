#include "test.h"

// Regression test: strclone() once treated a length of 0 as "copy the whole
// string", so an empty field copied the rest of the line ("k =, a" gave the
// value ", a").

static void
check_attrs(struct Canfigger *node, const char *const *expected, size_t n)
{
  size_t i = 0;
  char *attr = NULL;
  canfigger_free_current_attr_str_advance(node->attributes, &attr);
  while (attr)
  {
    fprintf(stderr, "%s: attr [%s]\n", node->key, attr);
    assert(i < n);
    assert(strcmp(attr, expected[i]) == 0);
    i++;
    canfigger_free_current_attr_str_advance(node->attributes, &attr);
  }
  assert(i == n);
}

int
main(void)
{
  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/empty_fields.conf", ',');
  assert(list);

  assert(strcmp(list->key, "") == 0);
  assert(strcmp(list->value, "foo") == 0);
  assert(list->attributes == NULL);
  canfigger_free_current_key_node_advance(&list);

  static const char *const k_attrs[] = { "a" };
  assert(strcmp(list->key, "k") == 0);
  assert(strcmp(list->value, "") == 0);
  check_attrs(list, k_attrs, ARRAY_SIZE(k_attrs));
  canfigger_free_current_key_node_advance(&list);

  static const char *const attrs_attrs[] = { "a", "", "b" };
  assert(strcmp(list->key, "attrs") == 0);
  assert(strcmp(list->value, "v") == 0);
  check_attrs(list, attrs_attrs, ARRAY_SIZE(attrs_attrs));
  canfigger_free_current_key_node_advance(&list);

  static const char *const trailing_attrs[] = { "a" };
  assert(strcmp(list->key, "trailing") == 0);
  assert(strcmp(list->value, "v") == 0);
  check_attrs(list, trailing_attrs, ARRAY_SIZE(trailing_attrs));
  canfigger_free_current_key_node_advance(&list);

  static const char *const tabbed_attrs[] = { "a", "", "b" };
  assert(strcmp(list->key, "tabbed") == 0);
  assert(strcmp(list->value, "v") == 0);
  check_attrs(list, tabbed_attrs, ARRAY_SIZE(tabbed_attrs));
  canfigger_free_current_key_node_advance(&list);

  assert(list == NULL);
  return 0;
}
