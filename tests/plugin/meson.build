libzathura_plugin = library(
  'plugin',
  files('plugin.c'),
  dependencies: build_dependencies + [libzathura_gtk_dependency],
  include_directories: include_directories,
  c_args: defines + flags,
  name_prefix: ''
)
