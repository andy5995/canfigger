# canfigger v0.1.999

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

# key, value with 2 attributes
dhcp-range = 192.168.0.50, 192.168.0.150, 12

# key, value with 9 attributes
solar_system = sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Neptune, Uranus, Pluto
```
## API documentation

See the annotated declarations in [/canfigger.h](https://github.com/andy5995/canfigger/blob/trunk/tests/test_parse_file.c)

## Examples

* [tests/test_parse_file.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_parse_file.c)
* [tests/test_multiple_attributes.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_multiple_attributes.c)

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
