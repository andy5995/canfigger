#include "test.h"

int
main(void)
{
  const struct expected
  {
    const char *key;
    const char *value;
    const char *attribute;
  } data[] = {
    {"🐢👀🍻", "some chars", "unicode"},
    {"👀🍻💯", NULL, NULL},
    {"🍻💯💣", "looks like a 💣",
     "Some cartoon characters will be very unhappy."},
    /* Trailing ASCII spaces after a 4-byte UTF-8 sequence: truncate_whitespace
     * must not walk back into the multibyte bytes. */
    {"💣💯👀", "emoji 💣", NULL},
  };

  struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/test_unicode.conf", ';');
  assert(list);

  int i = 0;
  while (list)
  {
    char *attr = NULL;
    canfigger_free_current_attr_str_advance(list->attributes, &attr);

    fprintf(stderr, "\nKey: %s | Expected: %s\nValue: %s | Expected: %s\nAttribute: %s | Expected: %s\n",
            list->key, data[i].key,
            list->value ? list->value : "NULL", data[i].value ? data[i].value : "NULL",
            attr ? attr : "NULL", data[i].attribute ? data[i].attribute : "NULL");

    assert(strcmp(data[i].key, list->key) == 0);
    assert(strcmp(data[i].value ? data[i].value : "NULL",
                  list->value ? list->value : "NULL") == 0);
    assert(strcmp(data[i].attribute ? data[i].attribute : "NULL",
                  attr ? attr : "NULL") == 0);
    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  assert(list == NULL);
  assert(i == (int) ARRAY_SIZE(data));

  return 0;
}
