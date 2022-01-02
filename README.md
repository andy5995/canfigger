# canfigger v0.2.0999

Simple configuration file parser library

[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml

website: https://github.com/andy5995/canfigger

This library contains a function that parses simple configuration files
that use a key/value pair with optional attributes.


```
foo = bar
blue = color, shiny
statement = hello world, obvious

# An option with no value or attributes
FeatureFooEnabled

# key, value with 9 attributes
solar_system = sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Neptune, Uranus, Pluto
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
// macro in string format containing the version of the library
CANFIGGER_VERSION

// macro; the max length of a line in a configuration file
__CFG_LEN_MAX_LINE (512 + 1)

typedef struct st_canfigger_attr_node st_canfigger_attr_node;
struct st_canfigger_attr_node
{
  char str[__CFG_LEN_MAX_LINE];
  st_canfigger_attr_node* next;
};

typedef struct st_canfigger_node st_canfigger_node;
struct st_canfigger_node
{
  // Contains the string that precedes the '=' sign
  char key[__CFG_LEN_MAX_LINE];

  // Contains the string between the '=' sign and the delimiter
  char value[__CFG_LEN_MAX_LINE];

  // Linked list of attributes
  st_canfigger_attr_node *attr_node;

  // A pointer to the next node in the list
  st_canfigger_node *next;
};

// Can be used interchangeably for code readability and developer preference
typedef st_canfigger_node st_canfigger_list;

// Frees the list returned by canfigger_parse_file()
void canfigger_free (st_canfigger_node * node);

// Frees the attribute node (which may be a linked list of attributes)
void
canfigger_free_attr (st_canfigger_attr_node * node);
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
