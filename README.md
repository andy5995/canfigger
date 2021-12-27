# canfigger v0.1.1999

Simple configuration file parser library

[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml

website: https://github.com/andy5995/canfigger

This library contains a function that parses simple configuration files
that use a key/value pair with an optional attribute.


```
foo = bar
blue = color, shiny
statement = hello world, obvious

# An option with no value or attributes
FeatureFooEnabled
```

## API

## canfigger_parse_file()

returns a [linked list](https://www.learn-c.org/en/Linked_lists), each
node in the list corresponds to a parsed line in your configuration
file.

## canfigger_free()

required to free the linked list

### Examples

See [tests/test_parse_file.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_parse_file.c)

### Structs, typedefs, and macros

```c
typedef struct st_canfigger_node st_canfigger_node;
struct st_canfigger_node
{
  // Contains the string that precedes the '=' sign
  char *key;

  // Contains the string between the '=' sign and the delimiter
  char *value;

  // Contains the string following the delimiter
  char *attribute;

  // A pointer to the next node in the list
  st_canfigger_node *next;
};

// Can be used interchangeably for code readability and developer preference
typedef st_canfigger_node st_canfigger_list;

// macro in string format containing the version of the library
CANFIGGER_VERSION

// macro; the max length of a line in a configuration file
__CFG_LEN_MAX_LINE (512 + 1)
```

## Building

```
meson _build
cd _build
ninja
```

## Run the tests

```
ninja test
meson test --setup=valgrind (requires valgrind to be installed)
```

## Install/Uninstall

```
ninja install
```

Or if you want to install without superuser privileges, first run

    meson -Dprefix=$HOME/.local _build

```
ninja uninstall
```
