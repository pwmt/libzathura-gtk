#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# See LICENSE file for license and copyright information

import sphinx_rtd_theme
import os

# -- General configuration ------------------------------------------------

extensions = [
    'sphinx.ext.todo',
    'hawkmoth'
]
source_suffix  = '.rst'
master_doc     = 'index'
templates_path = ['_templates']
exclude_patterns = ['_build']

pygments_style = 'sphinx'


# -- Project configuration ------------------------------------------------

project   = 'libzathura-gtk'
copyright = '2014-2020, pwmt.org'
version   = '0.0.0'
release   = '0.0.0'


# -- Options for HTML output ----------------------------------------------

html_theme        = 'sphinx_rtd_theme'
html_theme_path   = [sphinx_rtd_theme.get_html_theme_path()]
html_static_path  = ['_static']
htmlhelp_basename = 'libzathuragtkdoc'


# -- Options for hawkmoth -------------------------------------------------

cautodoc_root = os.path.abspath('../')
cautodoc_compat = 'javadoc-basic'
cautodoc_clang = "-DHAVE_CAIRO"
