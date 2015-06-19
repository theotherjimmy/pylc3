.SUFFIXES:

ifndef _ARCH
_ARCH := $(shell uname -s)
export _ARCH
endif

OBJDIR := _${_ARCH}

MAKETARGET = ${MAKE} --no-print-directory -C $@ -f ${CURDIR}/Makefile \
		SRCDIR=${CURDIR} SRCS="${SRCS}" python-tests="${python-tests}"  ${MAKECMDGOALS}

.PHONY: ${OBJDIR}

${OBJDIR} :
	+@[ -d $@ ] || mkdir -p $@
	+@${MAKETARGET} || true

Makefile : ;
%.mk :: ;
% :: $(OBJDIR) ; @:

.PHONY: clean
clean :
	rm -rf $(OBJDIR)
