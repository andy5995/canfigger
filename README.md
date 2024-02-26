[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]

# canfigger v0.3.0999

canfigger is a lightweight C language library designed to parse configuration
files. It provides functionality to read them and represent their contents as
a linked list of key-value pairs, along with associated attributes for each
pair.

* [website/API documentation and examples](https://github.com/andy5995/canfigger/)
* [source on GitHub](https://github.com/andy5995/canfigger/)

## Format

The following config file example represents the format handled by canfigger:

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
## Dependencies

None

## Building

    meson setup _build
    cd _build
    ninja

For configuration options, use `meson configure` (see the [Meson
docs](https://mesonbuild.com/) for detailed usage).

## Tests

    meson test (-v)

<!-- Add your project here if it has had at least one release -->
## Projects using canfigger

* [rmw](https://theimpossibleastronaut.github.io/rmw-website/)

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml
