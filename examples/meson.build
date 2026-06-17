snippet_examples = [
  'canfigger_config_dir',
  'canfigger_config_file',
  'canfigger_path_join',
  'canfigger_get_int_attrs',
  'canfigger_get_double_attrs',
]

foreach ex : snippet_examples
  executable(
    'example_' + ex,
    ex + '.c',
    dependencies: canfigger_dep,
    install: false,
  )
endforeach
