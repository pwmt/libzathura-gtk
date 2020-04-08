sphinx = find_program('sphinx-build')
pymod = import('python')

python = pymod.find_installation('python3')
python_required_modules = ['sphinx_rtd_theme', 'hawkmoth']

if sphinx.found()
	# Check required modules
	foreach p : python_required_modules
		script = 'import importlib.util; import sys; exit(1) if importlib.util.find_spec(\''+ p +'\') is None else exit(0)'
		if run_command(python, '-c', script).returncode() != 0
			error('Required Python module \'' + p + '\' not found')
		endif
	endforeach

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
