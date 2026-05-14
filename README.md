[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]
[![cirrus-badge]][cirrus-url]
[![windows-badge]][windows-url]

# canfigger v0.3.3

Canfigger is a lightweight C language library designed to parse configuration
files. It provides functionality to read them and represent their contents as
a linked list of key-value pairs, along with associated attributes for each
pair. It also includes utility functions for locating standard per-user
directories (config, data, cache) and joining paths, with support for XDG on
Linux/macOS and the Windows CSIDL equivalents.

* [website/API documentation and examples](https://andy5995.github.io/canfigger/)
* [source on GitHub](https://github.com/andy5995/canfigger/)
* [releases](https://github.com/andy5995/canfigger/releases/)

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

## Platform path helpers

Canfigger provides helpers for locating standard per-user paths on Linux
(XDG), macOS, and Windows:

- `canfigger_config_dir(appname)` — returns the per-application config
  directory (`$XDG_CONFIG_HOME/appname` or `$HOME/.config/appname`)
- `canfigger_data_dir(appname)` — returns the per-application data
  directory (`$XDG_DATA_HOME/appname` or `$HOME/.local/share/appname`)
- `canfigger_cache_dir(appname)` — returns the per-application cache
  directory (`$XDG_CACHE_HOME/appname` or `$HOME/.cache/appname`)
- `canfigger_config_file(filename)` — returns a path directly under the
  base config directory, without an application subdirectory
  (`$XDG_CONFIG_HOME/filename` or `$HOME/.config/filename`); useful when
  the config file lives at the config root rather than in a per-application
  subdirectory
- `canfigger_path_join(dir, file)` — joins a directory and filename with
  the platform separator

See the API documentation for details.

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

## Example programs

Example programs will be built when you run `ninja`. If you want to try them
with a different config file, give them the name of a config file as an
argument.

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml
[cirrus-badge]:https://api.cirrus-ci.com/github/andy5995/canfigger.svg
[cirrus-url]:https://cirrus-ci.com/github/andy5995/canfigger
[windows-badge]:https://github.com/andy5995/canfigger/actions/workflows/windows.yml/badge.svg
[windows-url]:https://github.com/andy5995/canfigger/actions/workflows/windows.yml
