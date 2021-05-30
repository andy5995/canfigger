project('canfigger', 'c', version: '0.1')
add_global_arguments('-Wextra', '-fno-common', language : 'c')
compiler = meson.get_compiler('c')
if compiler.has_argument('-fstack-protector-strong')
	add_project_arguments('-fstack-protector-strong', language: 'c')
endif
if compiler.has_argument('-fanalyzer')
	add_project_arguments('-fanalyzer', language: 'c')
endif

extra_args = ['-DSOURCE_DIR="' + meson.source_root() + '"']

src = (meson.source_root() / 'canfigger.c')
buildtarget = both_libraries(
	meson.project_name(),
	src,
	version : meson.project_version(),
	soversion : '0',
	install: true)

e = executable('test_canfigger', 'test_canfigger.c', c_args : extra_args, link_with : buildtarget.get_static_lib())
test('test_canfigger', e)

pkg = import('pkgconfig')
pkg.generate(
	buildtarget,
	description : 'Library for parsing configuration files')

install_headers('canfigger.h')
