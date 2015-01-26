# See LICENSE file for license and copyright information

include ../config.mk
include ../common.mk
include ../colors.mk
include config.mk

PROJECT = example
SOURCE  = $(wildcard *.c)
OBJECTS = $(addprefix ${BUILDDIR_RELEASE}/,${SOURCE:.c=.o})
OBJECTS_DEBUG = $(addprefix ${BUILDDIR_DEBUG}/,${SOURCE:.c=.o})

ifneq (${WITH_CAIRO},0)
INCS += ${CAIRO_INC}
LIBS += ${CAIRO_LIB}
CPPFLAGS += -DHAVE_CAIRO
endif

all: options ${PROJECT}

options:
	${ECHO} ${PROJECT}.so build options:
	${ECHO} "CFLAGS  = ${CFLAGS}"
	${ECHO} "LDFLAGS = ${LDFLAGS}"
	${ECHO} "LIBS    = ${LIBS}"
	${ECHO} "CC      = ${CC}"

# release build

${OBJECTS}: ../config.mk config.mk

${BUILDDIR_RELEASE}/%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p ${DEPENDDIR}/$(dir $(abspath $@))
	@mkdir -p $(dir $(abspath $@))
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $< -MMD -MF ${DEPENDDIR}/$(abspath $@).dep

${BUILDDIR_RELEASE}/${BINDIR}/${PROJECT}: ${OBJECTS} dependencies
	$(call colorecho,CC,$@)
	@mkdir -p ${BUILDDIR_RELEASE}/${BINDIR}
	$(QUIET)${CC} ${SFLAGS} ${LDFLAGS} \
		-o ${BUILDDIR_RELEASE}/${BINDIR}/${PROJECT} ${OBJECTS} ${LIBS} ${LIBZATHURA_GTK_RELEASE}

${PROJECT}: ${BUILDDIR_RELEASE}/${BINDIR}/${PROJECT}

dependencies:
	$(QUIET)${MAKE} WITH_LIBFIU=1 -C .. release
	$(QUIET)${MAKE} -C ../tests plugin

# debug build

${OBJECTS_DEBUG}: ../config.mk config.mk

${BUILDDIR_DEBUG}/%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p ${DEPENDDIR}/$(dir $(abspath $@))
	@mkdir -p $(dir $(abspath $@))
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $< -MMD -MF ${DEPENDDIR}/$(abspath $@).dep

${BUILDDIR_DEBUG}/${BINDIR}/${PROJECT}: ${OBJECTS_DEBUG} dependencies-debug
	$(call colorecho,CC,$@)
	@mkdir -p ${BUILDDIR_DEBUG}/${BINDIR}
	$(QUIET)${CC} ${SFLAGS} ${LDFLAGS} \
		-o ${BUILDDIR_DEBUG}/${BINDIR}/${PROJECT} ${OBJECTS_DEBUG} ${LIBS} ${LIBZATHURA_GTK_DEBUG}

debug: ${BUILDDIR_DEBUG}/${BINDIR}/${PROJECT}

dependencies-debug:
	$(QUIET)${MAKE} WITH_LIBFIU=1 -C .. debug
	$(QUIET)${MAKE} -C ../tests plugin

# debugging

gdb: debug
	$(QUIET)${GDB} ${BUILDDIR_DEBUG}/${BINDIR}/${PROJECT}

# clean

clean:
	$(QUIET)rm -rf ${PROJECT}.so ${OBJECTS} .depend ${PROJECT}.gcda ${PROJECT}.gcno

.PHONY: all options clean debug run dependencies dependencies-debug gdb

-include $(wildcard .depend/*.dep)
