# See LICENSE file for license and copyright information

CHECK_INC ?= $(shell pkg-config --cflags check)
CHECK_LIB ?= $(shell pkg-config --libs check)

INCS += ${CHECK_INC} ${FIU_INC} -I ../libzathura-gtk
LIBS += ${CHECK_LIB} ${FIU_LIB} -Wl,--whole-archive -Wl,--no-whole-archive
LDFLAGS += -rdynamic -ldl

LIBZATHURA_RELEASE=../${BUILDDIR_RELEASE}/libzathura-gtk.a
LIBZATHURA_DEBUG=../${BUILDDIR_DEBUG}/libzathura-gtk.a
LIBZATHURA_GCOV=../${BUILDDIR_GCOV}/libzathura-gtk.a
#
# valgrind
VALGRIND = valgrind
VALGRIND_ARGUMENTS = --tool=memcheck --leak-check=yes --leak-resolution=high \
	--show-reachable=yes --log-file=libzathura-gtk-valgrind.log
VALGRIND_SUPPRESSION_FILE = libzathura-gtk.suppression
