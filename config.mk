# See LICENSE file for license and copyright information

PROJECT = libzathura-gtk

LIBZATHURA_GTK_VERSION_MAJOR = 0
LIBZATHURA_GTK_VERSION_MINOR = 0
LIBZATHURA_GTK_VERSION_REV   = 1

VERSION = ${LIBZATHURA_GTK_VERSION_MAJOR}.${LIBZATHURA_GTK_VERSION_MINOR}.${LIBZATHURA_GTK_VERSION_REV}

# If the API changes, the API version and the ABI version have to be bumped.
LIBZATHURA_GTK_VERSION_API = 1

# If the ABI breaks for any reason, this has to be bumped.
LIBZATHURA_GTK_VERSION_ABI = 1

# Rules for the SOMAJOR and SOMINOR.
# Before a release check perform the following checks against the last release:
# * If a function has been removed or the paramaters of a function have changed
#   bump SOMAJOR and set SOMINOR to 0.
# * If any of the exported datastructures have changed in a incompatible way
# 	bump SOMAJOR and set SOMINOR to 0.
# * If a function has been added bump SOMINOR.

SOMAJOR = 1
SOMINOR = 0
SOVERSION = ${SOMAJOR}.${SOMINOR}

# paths
PREFIX ?= /usr
LIBDIR ?= ${PREFIX}/lib
INCLUDEDIR ?= ${PREFIX}/include
DEPENDDIR=.depend
PLUGINDIR ?= ${PREFIX}/lib/zathura
BUILDDIR ?= build
BUILDDIR_RELEASE ?= ${BUILDDIR}/release
BUILDDIR_DEBUG ?= ${BUILDDIR}/debug
BUILDDIR_GCOV ?= ${BUILDDIR}/gcov
BINDIR ?= bin
VERSIONCHECK_DIR ?= .version-checks

# version checks
# If you want to disable any of the checks, set *_VERSION_CHECK to 0.

# glib
GLIB_VERSION_CHECK ?= 1
GLIB_MIN_VERSION = 2.28
GLIB_PKG_CONFIG_NAME = glib-2.0

# GTK
GTK_VERSION_CHECK ?= 1
GTK_MIN_VERSION = 3.0
GTK_PKG_CONFIG_NAME = gtk+-3.0

# cairo
CAIRO_VERSION_CHECK ?= 1
CAIRO_MIN_VERSION = 1.12.9
CAIRO_PKG_CONFIG_NAME = cairo

# pkg-config binary
PKG_CONFIG ?= pkg-config

# libs
LIBZATHURA_INC ?= $(shell ${PKG_CONFIG} --cflags libzathura)
LIBZATHURA_LIB ?= $(shell ${PKG_CONFIG} --libs libzathura)

GTK_INC ?= $(shell ${PKG_CONFIG} --cflags gtk+-3.0)
GTK_LIB ?= $(shell ${PKG_CONFIG} --libs gtk+-3.0)

GTHREAD_INC ?= $(shell ${PKG_CONFIG} --cflags gthread-2.0)
GTHREAD_LIB ?= $(shell ${PKG_CONFIG} --libs   gthread-2.0)

GLIB_INC ?= $(shell ${PKG_CONFIG} --cflags glib-2.0 gio-2.0)
GLIB_LIB ?= $(shell ${PKG_CONFIG} --libs glib-2.0 gio-2.0)

GMODULE_INC ?= $(shell ${PKG_CONFIG} --cflags gmodule-no-export-2.0)
GMODULE_LIB ?= $(shell ${PKG_CONFIG} --libs gmodule-no-export-2.0)

CAIRO_INC ?= $(shell ${PKG_CONFIG} --cflags cairo)
CAIRO_LIB ?= $(shell ${PKG_CONFIG} --libs cairo)

FIU_INC ?= $(shell ${PKG_CONFIG} --cflags libfiu)
FIU_LIB ?= $(shell ${PKG_CONFIG} --libs libfiu)

INCS = ${LIBZATHURA_INC} ${GTK_INC} ${GTHREAD_INC} ${GLIB_INC} ${GMODULE_INC} ${CAIRO_INC}
LIBS = ${LIBZATHURA_LIB} ${GTK_LIB} ${GTHREAD_LIB} ${GLIB_LIB} ${GMODULE_LIB} ${CAIRO_LIB} -lm

# pre-processor flags
CPPFLAGS += -DHAVE_CAIRO -D_FILE_OFFSET_BITS=64

# compiler flags
CFLAGS += -std=c11 -pedantic -Wall -Wextra -fPIC $(INCS)

# linker flags
LDFLAGS += -fPIC

# debug
DFLAGS = -O0 -g

# compiler
CC ?= gcc

# archiver
AR ?= ar

# strip
SFLAGS ?= -s

# gcov & lcov
GCOV_CFLAGS=-fprofile-arcs -ftest-coverage
GCOV_LDFLAGS=-fprofile-arcs
LCOV_OUTPUT=gcov
LCOV_EXEC=lcov
LCOV_FLAGS=--base-directory . --directory ${BUILDDIR_GCOV} --capture --rc \
					 lcov_branch_coverage=1 --output-file ${BUILDDIR_GCOV}/$(PROJECT).info
GENHTML_EXEC=genhtml
GENHTML_FLAGS=--rc lcov_branch_coverage=1 --output-directory ${LCOV_OUTPUT} ${BUILDDIR_GCOV}/$(PROJECT).info

# libfiu
WITH_LIBFIU ?= 1
FIU_RUN ?= fiu-run -x

# debugging
GDB ?= cgdb

# set to something != 0 if you want verbose build output
VERBOSE ?= 0

# enable colors
COLOR ?= 1
