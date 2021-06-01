incdir = include_directories('..')
test_lib = static_library(
	meson.project_name() + '_test',
	'test.c',
	include_directories : incdir,
	install: false)

test_lib = declare_dependency(link_with : test_lib)
main_lib = declare_dependency(link_with : buildtarget.get_static_lib())

e1 = executable(
	'test_parse_file', 'test_parse_file.c',
	include_directories : incdir,
	dependencies : [test_lib, main_lib])
test('test_parse_file', e1)

e1 = executable(
	'test_parse_file2', 'test_parse_file2.c',
	include_directories : incdir,
	dependencies : [test_lib, main_lib])
test('test_parse_file2', e1)
