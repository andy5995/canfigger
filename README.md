[![codeql-badge]][codeql-url]
[![actions-c-badge]][actions-c-url]
[![cirrus-badge]][cirrus-url]
[![windows-badge]][windows-url]

# canfigger v0.3.3

Canfigger is a C library for parsing configuration files. It reads a file and
returns the contents as a linked list of key-value nodes. Each node can carry
a list of attributes — extra values separated by a delimiter on the same line.
The library includes helpers for reading attributes as integers and for parsing
RGBA hex colors. It also provides functions for locating standard user
directories on Linux (XDG), macOS, and Windows.

* [website/API documentation and examples](https://andy5995.github.io/canfigger/)
* [source on GitHub](https://github.com/andy5995/canfigger/)
* [releases](https://github.com/andy5995/canfigger/releases/)

## Format

One entry per line — a key, an optional value, and optional attributes:

```
# key with value only
name = canfigger

# key with no value or attributes
FeatureFooEnabled

# key with value and integer attributes (readable with canfigger_get_int_attrs)
board = rect, 10, 20, 640, 480

# key with value and hex color attributes (readable with canfigger_parse_color_pair)
button = pair, #FFFF00, #000000
```

The delimiter between value and attributes defaults to `,` but can be changed:

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

Example programs are built alongside the library. Each demonstrates one API function:

- `example_canfigger_config_dir` — `canfigger_config_dir()`
- `example_canfigger_config_file` — `canfigger_config_file()`
- `example_canfigger_path_join` — `canfigger_path_join()`
- `example_canfigger_get_int_attrs` — `canfigger_get_int_attrs()`
- `example_canfigger_parse_color_hex` — `canfigger_parse_color_hex()`
- `example_canfigger_parse_color_pair` — `canfigger_parse_color_pair()`

See the [API documentation](https://andy5995.github.io/canfigger/) for inline usage examples.

[codeql-badge]: https://github.com/andy5995/canfigger/workflows/CodeQL/badge.svg
[codeql-url]: https://github.com/andy5995/canfigger/actions?query=workflow%3ACodeQL
[actions-c-badge]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml/badge.svg
[actions-c-url]: https://github.com/andy5995/canfigger/actions/workflows/c-cpp.yml
[cirrus-badge]:https://api.cirrus-ci.com/github/andy5995/canfigger.svg
[cirrus-url]:https://cirrus-ci.com/github/andy5995/canfigger
[windows-badge]:https://github.com/andy5995/canfigger/actions/workflows/windows.yml/badge.svg
[windows-url]:https://github.com/andy5995/canfigger/actions/workflows/windows.yml
