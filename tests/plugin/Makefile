# See LICENSE file for license and copyright information

include ../../config.mk
include ../../common.mk
include ../../colors.mk
include config.mk

PROJECT = plugin
SOURCE  = $(wildcard *.c)
OBJECTS = ${SOURCE:.c=.o}

ifneq (${WITH_CAIRO},0)
INCS += ${CAIRO_INC}
LIBS += ${CAIRO_LIB}
CPPFLAGS += -DHAVE_CAIRO
endif

all: options ${PROJECT}.so

options:
	${ECHO} ${PROJECT}.so build options:
	${ECHO} "CFLAGS  = ${CFLAGS}"
	${ECHO} "LDFLAGS = ${LDFLAGS}"
	${ECHO} "LIBS    = ${LIBS}"
	${ECHO} "CC      = ${CC}"

%.o: %.c
	$(ECHO) CC $<
	@mkdir -p .depend/$(dir $@)
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $< -MMD -MF .depend/$@.dep

${PROJECT}.so: options ${OBJECTS}
	$(ECHO) LD $@
	$(QUIET)${CC} -shared ${LDFLAGS} -o $@ $(OBJECTS) ${LIBS}

${OBJECTS}: ../../config.mk config.mk

clean:
	$(QUIET)rm -rf ${PROJECT}.so ${OBJECTS} .depend ${PROJECT}.gcda ${PROJECT}.gcno

.PHONY: all options clean debug run

-include $(wildcard .depend/*.dep)
