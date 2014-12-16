# See LICENSE file for license and copyright information

include ../config.mk
include ../common.mk
include ../colors.mk
include config.mk

PROJECT = example
SOURCE  = $(wildcard *.c)
OBJECTS = ${SOURCE:.c=.o}

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

%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p .depend/$(dir $@)
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $< -MMD -MF .depend/$@.dep

dependencies:
	$(QUIET)${MAKE} WITH_LIBFIU=1 -C .. release
	$(QUIET)${MAKE} -C ../tests plugin

${PROJECT}: options dependencies ${OBJECTS}
	$(call colorecho,LD,$@)
	$(QUIET)${CC} ${LDFLAGS} -o $@ $(OBJECTS) ${LIBS} ${LIBZATHURA_GTK_RELEASE}

${OBJECTS}: ../config.mk config.mk

clean:
	$(QUIET)rm -rf ${PROJECT}.so ${OBJECTS} .depend ${PROJECT}.gcda ${PROJECT}.gcno

.PHONY: all options clean debug run dependencies

-include $(wildcard .depend/*.dep)
