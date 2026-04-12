#include "test.h"

/* Verify that the last entry in a file with no trailing newline is not
 * silently dropped. */
int
main(void)
{
  const struct expected
  {
    const char *key;
    const char *value;
  } data[] = {
    {"alpha", "one"},
    {"beta",  "two"},
  };

  struct Canfigger *list =
    canfigger_parse_file(SOURCE_DIR "/test_no_trailing_newline.conf", ',');
  assert(list);

  int i = 0;
  while (list)
  {
    fprintf(stderr, "\nKey: %s | Expected: %s\nValue: %s | Expected: %s\n",
            list->key, data[i].key,
            list->value ? list->value : "NULL", data[i].value);

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp(data[i].value, list->value) == 0);
    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  assert(list == NULL);
  assert(i == (int) ARRAY_SIZE(data));

  return 0;
}
