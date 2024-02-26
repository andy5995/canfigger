incdir = include_directories('..')

main_lib = declare_dependency(link_with : buildtarget)

test_cases = [
  'parse_file',
  'file_open_err',
  'german',
  'colons',
  'unicode',
  'multiple_attributes',
  ]

foreach case : test_cases
  exe = executable(
    'test_' + case,
    case + '.c',
    include_directories : incdir,
    dependencies : [main_lib],
    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
  )
  test(case, exe)
endforeach
