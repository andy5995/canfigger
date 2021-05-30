# canfigger-0.1.0-dev

Library for parsing config files

[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml

This library will parse simple configuration files, using a key/value
pair with an optional attribute.

```
foo = bar
blue = green, color
# Commented line

FeatureFoo-enabled
```

## Example usage

```c
  st_canfigger_list *list = canfigger_parse_file ("../test_canfigger.conf", ',');

  // create a pointer to the head of the list before examining the list.
  st_canfigger_list *head = list;

  if (list == NULL)
  {
    fprintf (stderr, "Error");
    return -1;
  }

  while (list != NULL)
  {
    printf ("\n\
Key: %s\n\
Value: %s\n\
Attribute: %s\n", list->key, list->value, list->attribute);

    list = list->next;
  }

  // pass the head pointer to canfigger_free when done
  canfigger_free (head);
```

## API

**CANFIGGER_VERSION** String containing the the version of the library

`st_canfigger_list *canfigger_parse_file (const char *file, const char delimiter)`

`void canfigger_free (st_canfigger_node *node)`

## Building

```
mkdir _build
cmake -B _build
```

## Run the tests

```
make -C _build all test
```
