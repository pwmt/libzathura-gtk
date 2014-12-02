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

# libs
LIBZATHURA_INC ?= $(shell pkg-config --cflags libzathura)
LIBZATHURA_LIB ?= $(shell pkg-config --libs libzathura)

GTK_INC ?= $(shell pkg-config --cflags gtk+-3.0)
GTK_LIB ?= $(shell pkg-config --libs gtk+-3.0)

GTHREAD_INC ?= $(shell pkg-config --cflags gthread-2.0)
GTHREAD_LIB ?= $(shell pkg-config --libs   gthread-2.0)

GLIB_INC ?= $(shell pkg-config --cflags glib-2.0 gio-2.0)
GLIB_LIB ?= $(shell pkg-config --libs glib-2.0 gio-2.0)

GMODULE_INC ?= $(shell pkg-config --cflags gmodule-no-export-2.0)
GMODULE_LIB ?= $(shell pkg-config --libs gmodule-no-export-2.0)

CAIRO_INC ?= $(shell pkg-config --cflags cairo)
CAIRO_LIB ?= $(shell pkg-config --libs cairo)

FIU_INC ?= $(shell pkg-config --cflags libfiu)
FIU_LIB ?= $(shell pkg-config --libs libfiu)

INCS = ${GTK_INC} ${GTHREAD_INC} ${GLIB_INC} ${GMODULE_INC} ${CAIRO_INC}
LIBS = ${GTK_LIB} ${GTHREAD_LIB} ${GLIB_LIB} ${GMODULE_LIB} ${CAIRO_LIB} -lm

# flags
CFLAGS += -std=c99 -pedantic -Wall -Wextra -fPIC $(INCS)

# linker flags
LDFLAGS += -fPIC

# debug
DFLAGS = -O0 -g

# compiler
CC ?= gcc

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

# set to something != 0 if you want verbose build output
VERBOSE ?= 0

# enable colors
COLOR ?= 1
