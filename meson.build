project('canfigger', 'c', version: '0.1')
extra_args = ['-DSOURCE_DIR="' + meson.source_root() + '"']
add_global_arguments('-Wextra', '-fno-common', extra_args, language : 'c')
compiler = meson.get_compiler('c')
if compiler.has_argument('-fstack-protector-strong')
	add_project_arguments('-fstack-protector-strong', language: 'c')
endif
if compiler.has_argument('-fanalyzer')
	add_project_arguments('-fanalyzer', language: 'c')
endif

src = (meson.source_root() / 'canfigger.c')
buildtarget = both_libraries(
	meson.project_name(),
	src,
	version : meson.project_version(),
	soversion : '0',
	install: true)

pkg = import('pkgconfig')
pkg.generate(
	buildtarget,
	description : 'Library for parsing configuration files')

install_headers('canfigger.h')

subdir('tests')