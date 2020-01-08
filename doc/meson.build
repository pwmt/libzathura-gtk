sphinx = find_program('sphinx-build')
pymod = import('python')
python = pymod.find_installation()

hawkmoth = python.dependency('hawkmoth')
message(hawkmoth.found())

if sphinx.found() and hawkmoth.found()
  doc_sources = [
    'index.rst',
    'installation.rst',
    'api/index.rst',
    'development/contributing.rst',
    'development/index.rst',
  ]
  custom_target('html',
    command: [
      sphinx,
      '-b', 'html',
      '-D', 'version=' + version,
      '-D', 'release=' + version,
      meson.current_source_dir(),
      '@OUTDIR@/html'
    ],
    output: ['html'],
    input: [
      'conf.py',
    ] + doc_sources,
    build_by_default: false,
    install: false,
  )

  custom_target('man pages',
    command: [
      sphinx,
      '-b', 'man',
      '-D', 'version=' + version,
      '-D', 'release=' + version,
      join_paths(meson.current_source_dir(), 'man'),
      '@OUTDIR@/man'
    ],
    output: ['man'],
    input: [
      'man/conf.py'
    ],
    build_by_default: false,
    install: true,
    install_dir: [
      join_paths(get_option('mandir'), 'man3'),
    ]
  )
endif
