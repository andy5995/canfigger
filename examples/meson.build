snippet_examples = [
  'canfigger_config_dir',
  'canfigger_config_file',
  'canfigger_find_config_file',
  'canfigger_path_join',
]

foreach ex : snippet_examples
  executable(
    'example_' + ex,
    ex + '.c',
    dependencies: canfigger_dep,
    install: false,
  )
endforeach
