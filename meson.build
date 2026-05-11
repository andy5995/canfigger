project(
  'canfigger',
  'c',
  version: '0.3.2',
  meson_version: '>= 0.48.0',
  license: 'MIT',
  default_options: ['warning_level=3', 'c_std=c99'],
)

cc = meson.get_compiler('c')
extra_flags = [
  '-fno-common',
  '-Wshadow',
  '-Wstrict-overflow=2',
  '-Wformat-security',
  '-Wformat-overflow=2',
  '-Wunused-result',
  '-Werror=odr',
  '-Werror=lto-type-mismatch',
  '-Werror=strict-aliasing',
]

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')

# Suppress MSVC C4996 deprecation warnings for standard C functions like strerror()
if cc.get_id() == 'msvc'
  add_project_arguments('/D_CRT_SECURE_NO_WARNINGS', language: 'c')
endif

# Parse version components
version_components = meson.project_version().split('.')
major_version = version_components[0]
minor_version = version_components[1]
patch_version = version_components[2]

conf = configuration_data()
conf.set('CANFIGGER_VERSION_MAJOR', major_version)
conf.set('CANFIGGER_VERSION_MINOR', minor_version)
conf.set('CANFIGGER_VERSION_PATCH', patch_version)
config_h = configure_file(output: 'canfigger_version.h', configuration: conf)

src = ('canfigger.c')
buildtarget = library(
  meson.project_name(),
  src,
  version: meson.project_version(),
  install: not meson.is_subproject(),
)

# How to use in a superproject and other info
# https://mesonbuild.com/Subprojects.html
canfigger_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: buildtarget,
)

if not meson.is_subproject()
  if get_option('build_examples')
    examples = ['example-01', 'example-02']
    foreach ex : examples
      example = executable(
        ex,
        ex + '.c',
        dependencies: canfigger_dep,
        c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir()),
      )
      if get_option('build_tests')
        test('test_' + ex, example)
      endif
    endforeach
  endif

  pkg = import('pkgconfig')
  pkg.generate(
    buildtarget,
    description: 'Simple configuration file parser library',
    subdirs: meson.project_name(),
  )

  install_headers('canfigger.h', config_h, subdir: meson.project_name())

  install_subdir(
    join_paths('docs'),
    install_dir: join_paths(get_option('docdir'), 'html'),
    strip_directory: true,
  )

  install_data(
    files(
      'ChangeLog.txt',
      'LICENSE',
      'README.md',
      'example-01.c',
      'example-01.conf',
      'example-02.c',
      'example-02.conf',
    ),
    install_dir: get_option('docdir'),
  )

  if get_option('build_tests')
    subdir('tests')
  endif
endif
