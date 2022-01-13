project(
  'canfigger',
  'c',
  version: '0.2.0999',
  meson_version : '>= 0.48.0',
  default_options: [
    'warning_level=3',
    'default_library=both']
  )

cc = meson.get_compiler('c')
extra_flags = [
  '-fno-common',
  '-Wshadow',
  '-Wstrict-overflow=5',
  '-fanalyzer',
  '-Wformat-security',
  '-Wformat-overflow=2',
  '-DCANFIGGER_VERSION="@0@"'.format(meson.project_version())
]

foreach cflag : extra_flags
  if cc.has_argument(cflag)
    add_project_arguments(cflag, language: 'c')
  endif
endforeach

inc = include_directories('.')

src = ('canfigger.c')
buildtarget = both_libraries(
  meson.project_name(),
  src,
  include_directories : inc,
  version : meson.project_version(),
  soversion : '0',
  install: true)

dep_canfigger_static = declare_dependency(include_directories : inc,
  link_with : buildtarget.get_static_lib())
dep_canfigger_shared = declare_dependency(include_directories : inc,
  link_with : buildtarget.get_shared_lib())

pkg = import('pkgconfig')
pkg.generate(
  buildtarget,
  description : 'Simple configuration file parser library')

install_headers('canfigger.h')
install_data(files('README.md', 'LICENSE', 'ChangeLog'),
        install_dir : get_option('docdir'))

subdir('tests')
