project(
  'canfigger',
  'c',
  version: '0.2.0999',
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
buildtarget = library(
  meson.project_name(),
  src,
  include_directories : inc,
  version : meson.project_version(),
  soversion : '0',
  install: not meson.is_subproject())

# How to use in a superproject and other info
# https://mesonbuild.com/Subprojects.html
dep_canfigger = declare_dependency(include_directories : inc,
  link_with : buildtarget)

if not meson.is_subproject()
  pkg = import('pkgconfig')
  pkg.generate(
    buildtarget,
    description : 'Simple configuration file parser library')

  install_headers('canfigger.h')
  install_data(files('README.md', 'LICENSE', 'ChangeLog'),
          install_dir : get_option('docdir'))
endif

subdir('tests')
