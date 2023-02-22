incdir = include_directories('..')

main_lib = declare_dependency(link_with : buildtarget)

test_cases = [
  'test_parse_file',
  'test_parse_file2',
  'test_parse_file_colons',
  'test_unicode',
  'test_multiple_attributes',
  ]

foreach case : test_cases
  exe = executable(
    case,
    case + '.c',
    include_directories : incdir,
    dependencies : [main_lib],
    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
  )
  test(case, exe)
endforeach
