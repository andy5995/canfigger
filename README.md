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

# key, value with 2 attributes
dhcp-range = 192.168.0.50, 192.168.0.150, 12

# key, value with 9 attributes
solar_system = sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Neptune, Uranus, Pluto
```
## API documentation

See the annotated declarations in [/canfigger.h](https://github.com/andy5995/canfigger/blob/340f559a594ceb5e0252e35a38665503d5fc438c/canfigger.h)

## Examples

```c
st_canfigger_list *config = canfigger_parse_file("path/to/config.conf", ',');
while (config != NULL) {
    printf("Key: %s, Value: %s\n", config->key, config->value);

    // Process attributes if necessary
    st_canfigger_attr_node *attr = config->attr_node;
    while (attr) {
        printf("Attribute: %s\n", attr->str);
        attr = canfigger_get_next_attr(attr);
    }

    // Move to the next node and automatically free the current node
    config = canfigger_get_next_key(config);
}
```

* [tests/test_parse_file.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_parse_file.c)
* [tests/test_multiple_attributes.c](https://github.com/andy5995/canfigger/blob/trunk/tests/test_multiple_attributes.c)

## Building

```
meson _build
cd _build
ninja
```

By default, only the shared library will get built. To build both:

    meson configure -Ddefault_library=both

or to only build the static library:

    meson configure -Ddefault_libary=static

And then run `ninja`.

## Run the tests

    ninja test

## Install/Uninstall

```
ninja install
```

Or if you want to install without superuser privileges, first run

    meson -Dprefix=$HOME/.local _build

```
ninja uninstall
```

<!-- Add your project here if it has had at least one release -->
## Projects using canfigger

* [rmw](https://theimpossibleastronaut.github.io/rmw-website/)

