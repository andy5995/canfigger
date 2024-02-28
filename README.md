[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]
[![cirrus-badge]][cirrus-url]

# canfigger v0.3.0

Canfigger is a lightweight C language library designed to parse configuration
files. It provides functionality to read them and represent their contents as
a linked list of key-value pairs, along with associated attributes for each
pair.

* [website/API documentation and examples](https://andy5995.github.io/canfigger/)
* [source on GitHub](https://github.com/andy5995/canfigger/)
* [releases](https://github.com/andy5995/canfigger/releases)

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

You can change the attribute delimiter character by passing it as the second
argument:

    canfigger_parse_file(filename_ptr, ':');

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

## Example program

An example program will be built when you run `ninja`. It will read
`examplerc` in the source root directory. If you want to try it with a
different config file, give `example` the name of a config file as an
argument.

<!-- Add your project here if it has had at least one release -->
## Projects using canfigger

* [rmw](https://theimpossibleastronaut.github.io/rmw-website/)

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml
[cirrus-badge]:https://api.cirrus-ci.com/github/andy5995/canfigger.svg
[cirrus-url]:https://cirrus-ci.com/github/andy5995/canfigger

