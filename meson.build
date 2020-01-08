project('libzathura-gtk', 'c',
  version: '0.0.1',
  meson_version: '>=0.49',
  default_options: 'c_std=c11',
  license: 'zlib'
)

project_description = 'GTK+ Widgets for libzathura'
project_url = 'https://pwmt.org/projects/libzathura-gtk'

version = meson.project_version()
version_array = version.split('.')

# Rules for so_major and so_minor:
# Before a release perform the following checks against the last release:
# * If a function has been removed or the paramaters of a function have changed
#   bump SOMAJOR and set SOMINOR to 0.
# * If any of the exported datastructures have changed in a incompatible way
#   bump SOMAJOR and set SOMINOR to 0.
# * If a function has been added bump SOMINOR.
so_major = 1
so_minor = 1
so_version = '@0@.@1@'.format(so_major, so_minor)

conf_data = configuration_data()
conf_data.set('ZVMAJOR', version_array[0])
conf_data.set('ZVMINOR', version_array[1])
conf_data.set('ZVREV',   version_array[2])
conf_data.set('ZVAPI',   so_major)
conf_data.set('ZVABI',   so_minor)
conf_data.set('version',  version)

cc = meson.get_compiler('c')

prefix = get_option('prefix')
localedir = get_option('localedir')

# required dependencies
libm = cc.find_library('m', required: false)
libzathura = dependency('libzathura', version: '>=0.0.1', fallback: ['libzathura', 'libzathura_dependency'])
glib = dependency('glib-2.0', version: '>=2.32.4')
gtk3 = dependency('gtk+-3.0', version: '>=3.20')
rsvg = dependency('librsvg-2.0')

build_dependencies = [libzathura, libm, glib, gtk3, rsvg]
pc_requires = ['glib-2.0', 'gtk+-3.0', 'libzathura']

# defines
defines = [
  '-DGETTEXT_PACKAGE="libzathura-gtk-@0@"'.format(so_major),
  '-DLOCALEDIR="@0@"'.format(join_paths(prefix, localedir)),
  '-D_DEFAULT_SOURCE',
]

if rsvg.found()
  defines += '-DLIBZATHURA_GTK_ICONDIR="/tmp"'
endif


# compile flags
flags = [
  '-Wall',
  '-Wextra',
  '-pedantic',
  '-Wmissing-declarations',
  '-Werror=implicit-function-declaration',
  '-Wno-unused-variable',
  '-Wno-unused-parameter',
  '-Werror=vla',
]

if cc.get_id() == 'clang'
  flags += '-Wno-unused-command-line-argument'
endif

flags = cc.get_supported_arguments(flags)

# optional dependencies
cairo = dependency('cairo', version: '>=1.14', required: get_option('enable-cairo'))
libfiu = dependency('libfiu', version: '>=0.91', required: get_option('enable-libfiu'))

if cairo.found()
  build_dependencies += cairo
  defines += '-DHAVE_CAIRO'
endif

if get_option('enable-libfiu') and libfiu.found()
  build_dependencies += libfiu
  defines += '-DWITH_LIBFIU'
endif

# generate version header file
version_header = configure_file(
  input: 'libzathura-gtk/version.h.in',
  output: 'version.h',
  configuration: conf_data
)

include_directories = [
  include_directories('.')
]

# source files
sources = files(
  'libzathura-gtk/utils.c',
  'libzathura-gtk/document/callbacks.c',
  'libzathura-gtk/document/document.c',
  'libzathura-gtk/document/grid.c',
  'libzathura-gtk/page/annotations/annotation-3d.c',
  'libzathura-gtk/page/annotations/annotation-caret.c',
  'libzathura-gtk/page/annotations/annotation-circle.c',
  'libzathura-gtk/page/annotations/annotation-file-attachment.c',
  'libzathura-gtk/page/annotations/annotation-free-text.c',
  'libzathura-gtk/page/annotations/annotation-highlight.c',
  'libzathura-gtk/page/annotations/annotation-ink.c',
  'libzathura-gtk/page/annotations/annotation-line.c',
  'libzathura-gtk/page/annotations/annotation-link.c',
  'libzathura-gtk/page/annotations/annotation-movie.c',
  'libzathura-gtk/page/annotations/annotation-polygon.c',
  'libzathura-gtk/page/annotations/annotation-polyline.c',
  'libzathura-gtk/page/annotations/annotation-printer-mark.c',
  'libzathura-gtk/page/annotations/annotation-screen.c',
  'libzathura-gtk/page/annotations/annotation-sound.c',
  'libzathura-gtk/page/annotations/annotation-square.c',
  'libzathura-gtk/page/annotations/annotation-squiggly.c',
  'libzathura-gtk/page/annotations/annotation-stamp.c',
  'libzathura-gtk/page/annotations/annotation-strike-out.c',
  'libzathura-gtk/page/annotations/annotation-text.c',
  'libzathura-gtk/page/annotations/annotation-underline.c',
  'libzathura-gtk/page/annotations/annotation-watermark.c',
  'libzathura-gtk/page/annotations/annotation-widget.c',
  'libzathura-gtk/page/annotations/annotation.c',
  'libzathura-gtk/page/annotations/icons.c',
  'libzathura-gtk/page/annotations/overlay.c',
  'libzathura-gtk/page/annotations/utils.c',
  'libzathura-gtk/page/callbacks.c',
  'libzathura-gtk/page/form-fields/editor.c',
  'libzathura-gtk/page/form-fields/form-field-button.c',
  'libzathura-gtk/page/form-fields/form-field-choice.c',
  'libzathura-gtk/page/form-fields/form-field-text.c',
  'libzathura-gtk/page/page.c',
  'libzathura-gtk/page/utils.c',
  'libzathura-gtk/widgets/rotated-bin.c',
)

# header files to install
headers = {
  'libzathura': files(
    'libzathura-gtk/libzathura-gtk.h',
    'libzathura-gtk/macros.h',
    'libzathura-gtk/utils.h'
  ),
  'libzathura/document': files(
    'libzathura-gtk/document/internal.h',
    'libzathura-gtk/document/types.h',
    'libzathura-gtk/document/callbacks.h',
    'libzathura-gtk/document/document.h',
    'libzathura-gtk/document/grid.h'
  ),
  'libzathura/page': files(
    'libzathura-gtk/page/callbacks.h',
    'libzathura-gtk/page/internal.h',
    'libzathura-gtk/page/page.h',
    'libzathura-gtk/page/utils.h'
  ),
  'libzathura/page/annotations': files(
    'libzathura-gtk/page/annotations/annotation.h',
    'libzathura-gtk/page/annotations/icons.h',
    'libzathura-gtk/page/annotations/overlay.h',
    'libzathura-gtk/page/annotations/utils.h',
    'libzathura-gtk/page/annotations/annotation-3d.h',
    'libzathura-gtk/page/annotations/annotation-caret.h',
    'libzathura-gtk/page/annotations/annotation-circle.h',
    'libzathura-gtk/page/annotations/annotation-file-attachment.h',
    'libzathura-gtk/page/annotations/annotation-free-text.h',
    'libzathura-gtk/page/annotations/annotation-highlight.h',
    'libzathura-gtk/page/annotations/annotation-ink.h',
    'libzathura-gtk/page/annotations/annotation-line.h',
    'libzathura-gtk/page/annotations/annotation-link.h',
    'libzathura-gtk/page/annotations/annotation-movie.h',
    'libzathura-gtk/page/annotations/annotation-polygon.h',
    'libzathura-gtk/page/annotations/annotation-polyline.h',
    'libzathura-gtk/page/annotations/annotation-printer-mark.h',
    'libzathura-gtk/page/annotations/annotation-screen.h',
    'libzathura-gtk/page/annotations/annotation-sound.h',
    'libzathura-gtk/page/annotations/annotation-square.h',
    'libzathura-gtk/page/annotations/annotation-squiggly.h',
    'libzathura-gtk/page/annotations/annotation-stamp.h',
    'libzathura-gtk/page/annotations/annotation-strike-out.h',
    'libzathura-gtk/page/annotations/annotation-text.h',
    'libzathura-gtk/page/annotations/annotation-underline.h',
    'libzathura-gtk/page/annotations/annotation-watermark.h',
    'libzathura-gtk/page/annotations/annotation-widget.h'
  ),
  'libzathura/page/form-fields': files(
    'libzathura-gtk/page/form-fields/editor.h',
    'libzathura-gtk/page/form-fields/form-field-button.h',
    'libzathura-gtk/page/form-fields/form-field-choice.h',
    'libzathura-gtk/page/form-fields/form-field-text.h'
  ),
  'libzathura/widgets': files(
    'libzathura-gtk/widgets/rotated-bin.h'
  )
}

# libzathura-gtk library
libzathura_gtk = library(
  'zathura-gtk',
  sources,
  dependencies: build_dependencies,
  version: so_version,
  install: true,
  include_directories: include_directories,
  c_args: defines + flags,
  # gnu_symbol_visibility: 'hidden'
)

libzathura_gtk_dependency = declare_dependency(link_with: libzathura_gtk, include_directories: include_directories)

install_headers(version_header, subdir: 'libzathura-gtk')
foreach path, header_files: headers
  install_headers(header_files, subdir: path)
endforeach

# pkg-config file
pkg = import('pkgconfig')
pkg.generate(
  name: meson.project_name(),
  description: project_description,
  url: project_url,
  version: version,
  libraries: libzathura_gtk,
  requires: pc_requires
)

subdir('examples')
subdir('doc')
subdir('tests')
