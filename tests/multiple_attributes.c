#include "test.h"

int
main(void)
{
  const char *data1[] =
    { "one", "two", "three", "forty-four", "fifty five", "six", "7",
    "eight"
  };
  const char *data2[] =
    { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Neptune",
    "Uranus", "Pluto"
  };
  const char *data3[] = {
    "second",
    "after one",
  };

  // call the primary library function to read your config file
  struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/multiple_attributes.conf", ',');
  assert (list);

  int i = 0;
  while (list)
  {
    printf("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attributes->current);

    // assert (strcmp (data[i].key, list->key) == 0);
    // assert (strcmp (data[i].value, list->value) == 0);
    int j = 0;

    char *attr = NULL;
    //
    // Pass '&addr' to this function and it will get assigned an
    // attribute, or NULL if there are none.
    canfigger_free_current_attr_str_advance(list->attributes, &attr);
    while (attr)
    {
      fprintf(stderr, "attr: %s\n", list->attributes->current);
      switch (i)
      {
      case 0:
        assert(strcmp(data1[j], list->attributes->current) == 0);
        break;
      case 1:
        assert(strcmp(data2[j], list->attributes->current) == 0);
        break;
      case 2:
        assert(strcmp(data3[j], list->attributes->current) == 0);
        break;
      }
      j++;

      canfigger_free_current_attr_str_advance(list->attributes, &attr);
    }

    fprintf(stderr, "j: %d\n", j);
    if (j > 0)
    {
      switch (i)
      {
      case 0:
        assert(j == sizeof data1 / sizeof data1[0]);
        break;
      case 1:
        assert(j == sizeof data2 / sizeof data2[0]);
        break;
      case 2:
        assert(j == sizeof data3 / sizeof data3[0]);
        break;
      }
    }

    i++;

    canfigger_free_current_key_node_advance(&list);
  }

  assert(i == 3);

  return 0;
}
