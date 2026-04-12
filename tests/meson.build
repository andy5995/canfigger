test_cases = [
  'bom',
  'colons',
  'file_open_err',
  'german',
  'incomplete_parse_file',
  'multiple_attributes',
  'no_trailing_newline',
  'parse_file',
  'skip_attributes',
  'unicode',
  ]

foreach case : test_cases
  exe = executable(
    'test_' + case,
    case + '.c',
    dependencies : [canfigger_dep],
    c_args: '-DSOURCE_DIR="@0@"'.format(meson.current_source_dir())
  )
  test(case, exe)
endforeach
