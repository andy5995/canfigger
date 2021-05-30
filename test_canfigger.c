
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "canfigger.h"

struct expected {
  const char *key;
  const char *value;
  const char *attribute;
};
// #define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main() {

  const struct expected data[] = {
    { "foo", "bar", "" },
    { "blue", "green", "color" },
    { "FeatureFoo-enabled", "", "" },
  };

  st_canfigger_node *list = canfigger_parse_file ("../test_canfigger.conf", ',');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_node *head = list;

  if (list == NULL)
  {
    fprintf (stderr, "Error");
    return -1;
  }

  int i = 0;
  while (list != NULL)
  {
    printf ("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attribute);

    assert (strcmp (data[i].key, list->key) == 0);
    assert (strcmp (data[i].value, list->value) == 0);
    assert (strcmp (data[i].attribute, list->attribute) == 0);
    i++;

    list = list->next;
  }

  canfigger_free (head);

  return 0;
}
