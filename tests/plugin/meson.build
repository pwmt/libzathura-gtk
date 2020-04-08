libzathura_plugin = library(
  'plugin',
  files('plugin.c'),
  dependencies: build_dependencies + [libzathura],
  include_directories: include_directories,
  c_args: defines + flags + ['-DZATHURA_API_VERSION=1', '-DZATHURA_ABI_VERSION=1'],
  name_prefix: ''
)
