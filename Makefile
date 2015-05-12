# See LICENSE file for license and copyright information

include config.mk
include colors.mk
include common.mk

SOURCE          = $(wildcard \
                    ${PROJECT}/*.c \
                    ${PROJECT}/**/*.c \
                    ${PROJECT}/**/**/*.c)
OBJECTS         = $(addprefix ${BUILDDIR_RELEASE}/,${SOURCE:.c=.o})
OBJECTS_DEBUG   = $(addprefix ${BUILDDIR_DEBUG}/,${SOURCE:.c=.o})
OBJECTS_GCOV    = $(addprefix ${BUILDDIR_GCOV}/,${SOURCE:.c=.o})
HEADERS         = $(filter-out ${PROJECT}/version.h, \
                  $(filter-out ${PROJECT}/internal.h, \
                  $(filter-out ${PROJECT}/internal/*.h, \
                  $(filter-out ${PROJECT}/**/internal.h, \
                  $(filter-out ${PROJECT}/**/internal/*.h, \
                  $(wildcard \
                    ${PROJECT}/*.h \
                    ${PROJECT}/**/*.h \
                    ${PROJECT}/**/**/*.h \
                  ))))))
HEADERS_INSTALL = ${HEADERS} ${PROJECT}/version.h

DATA_ICONS      = $(wildcard \
                  data/icons/*.svg \
                  data/icons/**/*.svg \
                  data/icons/**/**/*.svg)
ICONS = $(subst ${ICONDIR_LOCAL},,${DATA_ICONS})

# libfiu support
ifneq (${WITH_LIBFIU},0)
INCS += ${FIU_INC}
LIBS += ${FIU_LIB}
CPPFLAGS += -DFIU_ENABLE
endif

# hidpi support
HIDPI_SUPPORT_CAIRO = $(shell ${PKG_CONFIG} --atleast-version=1.14 ${CAIRO_PKG_CONFIG_NAME} 1>&2 2> /dev/null; echo $$?)
HIDPI_SUPPORT_GTK = $(shell ${PKG_CONFIG} --atleast-version=3.10 ${GTK_PKG_CONFIG_NAME} 1>&2 2> /dev/null; echo $$?)
ifeq (${HIDPI_SUPPORT_CAIRO},0)
ifeq (${HIDPI_SUPPORT_GTK},0)
	CPPFLAGS += -DHAVE_HIDPI_SUPPORT
endif
endif

# icon dir
ifeq (,$(findstring -DLIBZATHURA_GTK_ICONDIR,${CPPFLAGS}))
CPPFLAGS += -DLIBZATHURA_GTK_ICONDIR=\"${ICONDIR}\"
endif

all: options ${PROJECT}

options:
ifeq "$(VERBOSE)" "1"
	$(ECHO) ${PROJECT} build options:
	$(ECHO) "CFLAGS  = ${CFLAGS}"
	$(ECHO) "LDFLAGS = ${LDFLAGS}"
	$(ECHO) "DFLAGS  = ${DFLAGS}"
	$(ECHO) "CC      = ${CC}"
endif

# pkg-config based version checks
${VERSIONCHECK_DIR}/%: config.mk
	$(QUIET)test $($(*)_VERSION_CHECK) -eq 0 || \
		pkg-config --atleast-version $($(*)_MIN_VERSION) $($(*)_PKG_CONFIG_NAME) || ( \
		echo "The minium required version of $(*) is $($(*)_MIN_VERSION)" && \
		false \
	)
	@mkdir -p ${VERSIONCHECK_DIR}
	$(QUIET)touch $@

${PROJECT}/version.h: ${PROJECT}/version.h.in config.mk
	$(QUIET)sed 's/ZVMAJOR/${LIBZATHURA_VERSION_MAJOR}/' < ${PROJECT}/version.h.in | \
		sed 's/ZVMINOR/${LIBZATHURA_VERSION_MINOR}/' | \
		sed 's/ZVREV/${LIBZATHURA_VERSION_REV}/' | \
		sed 's/ZVAPI/${LIBZATHURA_VERSION_API}/' | \
		sed 's/ZVABI/${LIBZATHURA_VERSION_ABI}/' > ${PROJECT}/version.h

# release build

${OBJECTS}:  config.mk ${PROJECT}/version.h \
	${VERSIONCHECK_DIR}/GLIB ${VERSIONCHECK_DIR}/GTK ${VERSIONCHECK_DIR}/CAIRO

${BUILDDIR_RELEASE}/%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p ${DEPENDDIR}/$(dir $(abspath $@))
	@mkdir -p $(dir $(abspath $@))
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $< -MMD -MF ${DEPENDDIR}/$(abspath $@).dep

${PROJECT}: ${PROJECT}/version.h static shared

static: ${BUILDDIR_RELEASE}/${PROJECT}.a
shared: ${BUILDDIR_RELEASE}/${PROJECT}.so.${SOVERSION}

${BUILDDIR_RELEASE}/${PROJECT}.a: ${OBJECTS}
	$(call colorecho,AR,$@)
	$(QUIET)${AR} rcs ${BUILDDIR_RELEASE}/${PROJECT}.a ${OBJECTS}

${BUILDDIR_RELEASE}/${PROJECT}.so.${SOVERSION}: ${OBJECTS}
	$(call colorecho,LD,$@)
	$(QUIET)${CC} -Wl,-soname,${PROJECT}.so.${SOMAJOR} -shared ${LDFLAGS} \
		-o ${BUILDDIR_RELEASE}/${PROJECT}.so.${SOVERSION} ${OBJECTS} ${LIBS}

release: options ${PROJECT}

# debug build

${OBJECTS_DEBUG}: config.mk ${PROJECT}/version.h \
	${VERSIONCHECK_DIR}/GLIB ${VERSIONCHECK_DIR}/GTK ${VERSIONCHECK_DIR}/CAIRO

${BUILDDIR_DEBUG}/%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p ${DEPENDDIR}/$(dir $(abspath $@))
	@mkdir -p $(dir $(abspath $@))
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} ${DFLAGS} \
		-o $@ $< -MMD -MF ${DEPENDDIR}/$(abspath $@).dep

${PROJECT}-debug: ${PROJECT}/version.h static-debug shared-debug

static-debug: ${BUILDDIR_DEBUG}/${PROJECT}.a
shared-debug: ${BUILDDIR_DEBUG}/${PROJECT}.so.${SOVERSION}

${BUILDDIR_DEBUG}/${PROJECT}.a: ${OBJECTS_DEBUG}
	$(call colorecho,AR,${PROJECT}.a)
	$(QUIET)${AR} rc ${BUILDDIR_DEBUG}/${PROJECT}.a ${OBJECTS_DEBUG}

${BUILDDIR_DEBUG}/${PROJECT}.so.${SOVERSION}: ${OBJECTS_DEBUG}
	$(call colorecho,LD,${PROJECT}.so.${SOMAJOR})
	$(QUIET)${CC} -Wl,-soname,${PROJECT}.so.${SOMAJOR} -shared ${LDFLAGS} \
		-o ${BUILDDIR_DEBUG}/${PROJECT}.so.${SOVERSION} ${OBJECTS_DEBUG} ${LIBS}

debug: options ${PROJECT}-debug

# gcov build

${OBJECTS_GCOV}: config.mk ${PROJECT}/version.h \
	${VERSIONCHECK_DIR}/GLIB ${VERSIONCHECK_DIR}/GTK ${VERSIONCHECK_DIR}/CAIRO

${BUILDDIR_GCOV}/%.o: %.c
	$(call colorecho,CC,$<)
	@mkdir -p ${DEPENDDIR}/$(dir $(abspath $@))
	@mkdir -p $(dir $(abspath $@))
	$(QUIET)${CC} -c ${CPPFLAGS} ${CFLAGS} ${GCOV_CFLAGS} ${DFLAGS} \
		-o $@ $< -MMD -MF ${DEPENDDIR}/$(abspath $@).dep

${PROJECT}-gcov: ${PROJECT}/version.h static-gcov shared-gcov

static-gcov: ${BUILDDIR_GCOV}/${PROJECT}.a
shared-gcov: ${BUILDDIR_GCOV}/${PROJECT}.so.${SOVERSION}

${BUILDDIR_GCOV}/${PROJECT}.a: ${OBJECTS_GCOV}
	$(call colorecho,AR,${PROJECT}.a)
	$(QUIET)${AR} rc ${BUILDDIR_GCOV}/${PROJECT}.a ${OBJECTS_GCOV}

${BUILDDIR_GCOV}/${PROJECT}.so.${SOVERSION}: ${OBJECTS_GCOV}
	$(call colorecho,LD,${PROJECT}.so.${SOMAJOR})
	$(QUIET)${CC} -Wl,-soname,${PROJECT}.so.${SOMAJOR} -shared ${LDFLAGS} ${GCOV_LDFLAGS} \
		-o ${BUILDDIR_GCOV}/${PROJECT}.so.${SOVERSION} ${OBJECTS_GCOV} ${LIBS}

gcov: options ${PROJECT}-gcov
	$(QUIET)${MAKE} -C tests run-gcov
	$(call colorecho,LCOV,"Analyse data")
	$(QUIET)${LCOV_EXEC} ${LCOV_FLAGS}
	$(call colorecho,LCOV,"Generate report")
	$(QUIET)${GENHTML_EXEC} ${GENHTML_FLAGS}

clean:
	$(call colorecho,RM, "Clean objects and builds")
	$(QUIET)rm -rf ${BUILDDIR}

	$(call colorecho,RM, "Clean pkg-config files")
	$(QUIET)rm -rf ${PROJECT}.pc

	$(call colorecho,RM, "Clean dependencies")
	$(QUIET)rm -rf ${DEPENDDIR}
	$(QUIET)rm -rf ${VERSIONCHECK_DIR}

	$(call colorecho,RM, "Clean distribution files")
	$(QUIET)rm -rf ${PROJECT}-${VERSION}.tar.gz
	$(QUIET)rm -rf ${PROJECT}.info
	$(QUIET)rm -rf ${PROJECT}/version.h

	$(call colorecho,RM, "Clean code analysis")
	$(QUIET)rm -rf ${LCOV_OUTPUT}
	$(QUIET)rm -rf gcov

	$(QUIET)${MAKE} -C tests clean
	$(QUIET)${MAKE} -C doc clean

doc:
	$(QUIET)${MAKE} -C doc

test: ${PROJECT}
	$(QUIET)${MAKE} -C tests run

${PROJECT}.pc: ${PROJECT}.pc.in config.mk
	$(QUIET)echo project=${PROJECT} > ${PROJECT}.pc
	$(QUIET)echo version=${VERSION} >> ${PROJECT}.pc
	$(QUIET)echo apiversion=${LIBZATHURA_VERSION_API} >> ${PROJECT}.pc
	$(QUIET)echo abiversion=${LIBZATHURA_VERSION_ABI} >> ${PROJECT}.pc
	$(QUIET)echo includedir=${INCLUDEDIR} >> ${PROJECT}.pc
	$(QUIET)echo plugindir=${PLUGINDIR} >> ${PROJECT}.pc
	$(QUIET)echo libdir=${LIBDIR} >> ${PROJECT}.pc
	$(QUIET)cat ${PROJECT}.pc.in >> ${PROJECT}.pc

install-static: static
	$(call colorecho,INSTALL,"Install static library")
	$(QUIET)mkdir -m 755 -p ${DESTDIR}${LIBDIR}
	$(QUIET)install -m 644 ${BUILDDIR_RELEASE}/${PROJECT}.a ${DESTDIR}${LIBDIR}

install-shared: shared
	$(call colorecho,INSTALL,"Install shared library")
	$(QUIET)mkdir -m 755 -p ${DESTDIR}${LIBDIR}
	$(QUIET)install -m 644 ${BUILDDIR_RELEASE}/${PROJECT}.so.${SOVERSION} ${DESTDIR}${LIBDIR}
	$(QUIET)ln -s ${PROJECT}.so.${SOVERSION} ${DESTDIR}${LIBDIR}/${PROJECT}.so.${SOMAJOR} || \
		echo "Failed to create ${PROJECT}.so.${SOMAJOR}. Please check if it exists and points to the correct version of ${PROJECT}.so."
	$(QUIET)ln -s ${PROJECT}.so.${SOVERSION} ${DESTDIR}${LIBDIR}/${PROJECT}.so || \
		echo "Failed to create ${PROJECT}.so. Please check if it exists and points to the correct version of ${PROJECT}.so."

install-icons:
	$(call colorecho,INSTALL,"Install icons")
	$(QUIET)mkdir -m 755 -p ${DESTDIR}${ICONDIR}
	$(QUIET)for icon in ${ICONS}; do \
		mkdir -m 755 -p ${DESTDIR}${ICONDIR}/`dirname $$icon`; \
		install -m 644 ${ICONDIR_LOCAL}/$$icon ${DESTDIR}${ICONDIR}/$$icon; \
	done

install: options install-static install-shared install-headers install-icons

install-headers: ${PROJECT}/version.h ${PROJECT}.pc
	$(call colorecho,INSTALL,"Install pkg-config file")
	$(QUIET)mkdir -m 755 -p ${DESTDIR}${LIBDIR}/pkgconfig
	$(QUIET)install -m 644 ${PROJECT}.pc ${DESTDIR}${LIBDIR}/pkgconfig
	$(call colorecho,INSTALL,"Install header files")
	$(QUIET)mkdir -m 755 -p ${DESTDIR}${INCLUDEDIR}/${PROJECT}
	$(QUIET)for header in ${HEADERS_INSTALL}; do \
		mkdir -m 755 -p ${DESTDIR}${INCLUDEDIR}/`dirname $$header`; \
		install -m 644 $$header ${DESTDIR}${INCLUDEDIR}/$$header; \
	done

uninstall: uninstall-headers uninstall-icons
	$(call colorecho,UNINSTALL,"Remove library files")
	$(QUIET)rm -f ${LIBDIR}/${PROJECT}.a ${LIBDIR}/${PROJECT}.so.${SOVERSION} \
		${LIBDIR}/${PROJECT}.so.${SOMAJOR} ${LIBDIR}/${PROJECT}.so

uninstall-headers:
	$(call colorecho,UNINSTALL,"Remove header files")
	$(QUIET)rm -rf ${INCLUDEDIR}/${PROJECT}
	$(call colorecho,UNINSTALL,"Remove pkg-config file")
	$(QUIET)rm -f ${LIBDIR}/pkgconfig/${PROJECT}.pc

uninstall-icons:
	$(call colorecho,UNINSTALL,"Remove icons")
	$(QUIET)rm -rf ${DATADIR}

.PHONY: all options clean debug doc test dist install install-headers \
	uninstall ninstall-headers ${PROJECT} ${PROJECT}-debug static shared \
	install-static install-shared

${DEPENDDIR}S = ${OBJECTS:.o=.o.dep}
DEPENDS = ${${DEPENDDIR}S:^=${DEPENDDIR}/}
-include $${DEPENDDIR}S}
