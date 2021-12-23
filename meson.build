project(
  'canfigger',
  'c',
  version: '0.1.1999',
  default_options: ['warning_level=3']
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

canfigger_dep = declare_dependency(include_directories : inc,
  link_with : buildtarget)

pkg = import('pkgconfig')
pkg.generate(
  buildtarget,
  description : 'Simple configuration file parser library')

install_headers('canfigger.h')
install_data(files('README.md', 'LICENSE', 'ChangeLog'),
        install_dir : get_option('docdir'))

subdir('tests')
