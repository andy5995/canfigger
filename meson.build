project(
  'canfigger',
  'c',
  version: '0.3.1999',
  meson_version : '>= 0.48.0',
  default_options: [
    'warning_level=3',
    'b_sanitize=address,undefined',
    'b_lundef=false'
    ]
  )

cc = meson.get_compiler('c')
extra_flags = [
  '-fno-common',
  '-Wshadow',
  '-Wstrict-overflow=5',
  '-Wformat-security',
  '-Wformat-overflow=2',
  '-Wunused-result'
]

add_project_arguments(cc.get_supported_arguments(extra_flags), language: 'c')

# Parse version components
version_components = meson.project_version().split('.')
major_version = version_components[0]
minor_version = version_components[1]
patch_version = version_components[2]

conf = configuration_data()
conf.set('CANFIGGER_VERSION_MAJOR', major_version)
conf.set('CANFIGGER_VERSION_MINOR', minor_version)
conf.set('CANFIGGER_VERSION_PATCH', patch_version)
config_h = configure_file(output : 'config.h', configuration : conf)

inc = include_directories('.')

src = ('canfigger.c')
buildtarget = library(
  meson.project_name(),
  src,
  include_directories : inc,
  version : meson.project_version(),
  install: not meson.is_subproject())

# How to use in a superproject and other info
# https://mesonbuild.com/Subprojects.html
canfigger_dep = declare_dependency(include_directories : inc,
  link_with : buildtarget)

if not meson.is_subproject()
  pkg = import('pkgconfig')
  pkg.generate(
    buildtarget,
    description : 'Simple configuration file parser library')

  install_headers('canfigger.h')

  install_subdir(
  join_paths('docs'),
  install_dir : join_paths(get_option('docdir'), 'html'),
  strip_directory : true
  )

  install_data(files(
    'ChangeLog.txt',
    'example.c',
    'examplerc',
    'LICENSE',
    'README.md',
    'ReleaseNotes.txt'
    ),
    install_dir : get_option('docdir'))
endif

if get_option('build_examples')
  example = executable(
    'example',
    'example.c',
    dependencies: canfigger_dep,
    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
    )
endif

if get_option('build_tests')
  subdir('tests')
endif
