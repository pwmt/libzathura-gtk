subdir('plugin')

check = dependency('check', version: '>=0.9.8', required: get_option('tests'))
if check.found()
  test_dependencies = [
    declare_dependency(link_with: libzathura_gtk),
    check
  ]

  # default timeout
  default_timeout = 60*60

  # Set plugin path for tests
  defines += [
    '-D_TEST_PLUGIN_DIR_PATH=@0@/@1@'.format(meson.build_root(), libzathura_plugin.outdir()),
    '-D_TEST_PLUGIN_FILE_PATH=@0@'.format(libzathura_plugin.full_path()),
    '-D_TEST_FILE_PATH=@0@'.format(meson.current_source_dir() / 'meson.build'),
  ]

  if get_option('enable-libfiu') and libfiu.found()
    fiu_run = find_program('fiu-run')
  else
    fiu_run = disabler()
  endif

  components = {
    'document': ['document.c'],
    'page': ['page.c']
  }

  foreach name, sources: components
    exec = executable('test-' + name,
    sources + ['utils.c', 'tests.c'],
    dependencies: build_dependencies + test_dependencies,
    include_directories: include_directories,
    c_args: defines + flags
    )

    if get_option('enable-libfiu') and fiu_run.found() and libfiu.found()
      test(name, fiu_run,
        args: ['-x', exec],
        timeout: default_timeout
      )
    else
      test(name, exec,
        timeout: default_timeout
      )
    endif
  endforeach
endif
