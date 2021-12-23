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
that use a key/value pair with an optional attribute. Convenience
function are also provided that can retrieve a user's home, config, and
data directory, and convert '$HOME', '$UID', and '~' to their literal
counterparts.

```
foo=bar
blue=color,shiny

# Spaces adjacent to the '=' or the attribute delimiter ',' will be ignored.
# Leading tabs will be ignored.
		statement = hello world , obvious

# An option with no value or attributes
FeatureFooEnabled
```

`canfigger_parse_file()` returns a [linked
list](https://www.learn-c.org/en/Linked_lists), each node in the list
corresponds to a parsed line in your configuration file.

## Example usage

See [tests/test_parse_file.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_parse_file.c)

`canfigger_get_directories` returns a struct containing the absolute
path of the user's home, dataroot, and configroot directories. If
$XDG_DATA_HOME or $XDG_CONFIG_HOME exist as environmental variables,
those will be used. Otherwise dataroot will be appended to $HOME as
'/.local/share' and configroot will be appended as '/.config'.

See [tests/test_get_directories.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_get_directories.c)

## API

```c
//
// Opens a config file and returns a memory-allocated linked list
// that must be freed later (see canfiggerfree())
//
// Each node is of type st_canfigger_node.
st_canfigger_list *canfigger_parse_file (const char *file,
                                         const char delimiter);


//
// Frees the list returned by canfigger_parse_file()
void canfigger_free (st_canfigger_node * node);


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


String containing the version of the library
CANFIGGER_VERSION

// Holds paths to frequently used directories
struct st_canfigger_directory
{
  const char *home;
  char configroot[PATH_MAX];
  char dataroot[PATH_MAX];
};

Example: See tests/test_get_directories


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
