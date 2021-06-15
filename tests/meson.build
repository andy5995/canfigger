incdir = include_directories('..')
vg = find_program('valgrind', required : false)

# This is only set when running under Valgrind test setup.
# env = environment()
# env.set('TEST_ENV', '1')

if vg.found()
add_test_setup('valgrind',
  exe_wrapper : [vg, '--error-exitcode=1', '--leak-check=full'],
# env : env
  )
endif

main_lib = declare_dependency(link_with : buildtarget.get_static_lib())

test_cases = [
  'test_parse_file',
  'test_parse_file2',
  'test_parse_file_colons',
  'test_unicode'
  ]

foreach case : test_cases
  exe = executable(
    case,
    case + '.c',
    include_directories : incdir,
    dependencies : [main_lib]
  )
  test(case, exe)
endforeach
