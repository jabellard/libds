MAJOR := 0
MINOR := 1
NAME := ds
VERSION := ${MAJOR}.${MINOR}
TARGET := lib${NAME}.so
rebuildables = ${TARGET}

 # source and include tree locations
SOURCE_TREE := src
INCLUDE_TREE := include

vpath %.c ${SOURCE_TREE}
vpath %.h ${INCLUDE_TREE}

# compilation driver variables
CCD := gcc
CCD_FLAGS :=  -fpic -I${INCLUDE_TREE} -w
LIBS := -lm

# list of source files
source = $(wildcard ${SOURCE_TREE}/*.c)

# list of object files
objects = $(subst .c,.o,${source})
rebuildables += ${objects}

# list of external programs
ECHO := echo
CP := cp
GZIP := gzip
RM := rm
SED := sed
MAKE := make
AWK := awk
SORT := sort
PR := pr
CAT := cat
LDCONFIG := ldconfig
CHMOD := chmod
MKDIR := mkdir

.PHONY: all
all: ${TARGET}


${TARGET}: ${objects} ${LIBS}
	@${ECHO} "Linking executable object \"${TARGET}\"..."
	${CCD} -shared  -o $@ $^
	@${ECHO} "Done linking \"${TARGET}\"."	
	
%.o: %.c
	@${ECHO} "Building \"$@\" object..."
	${CCD} -c ${CCD_FLAGS}  -o $@ $<
	@${ECHO} "Done building \"$@\"."

%.d: %.c
	@${ECHO} "Generating include dependency file \"$@\"..."
	$(CCD) -M  ${CCD_FLAGS} $< > $@
	@${ECHO} "Done generating \"$@\"."
	
# automatically manage include dependencies for the source files
include_dependency_files := $(subst .c,.d,${source})
ifneq ($(MAKECMDGOALS),clean)
include ${include_dependency_files}
endif

rebuildables += ${include_dependency_files}

.PHONY: install
install:
	@${ECHO} "Installing library..."
	${MKDIR} /usr/include/${NAME}
	${CHMOD} 0755 /usr/include/${NAME}
	${CP} -f ${INCLUDE_TREE}/* /usr/include/${NAME}
	${CHMOD} 0644 /usr/include/${NAME}/*
	${CP} ${TARGET} /usr/lib
	${CHMOD} 0755 /usr/lib/${TARGET}
	${LDCONFIG}
	@${ECHO} "Done installing library."

.PHONY: uninstall
uninstall:
	@${ECHO} "Un-installing library..."
	${RM} /usr/lib/${TARGET}
	${LDCONFIG}
	${RM} -r /usr/include/${NAME}
	@${ECHO} "Done un-installing library."
		
.PHONY: clean
clean:
	@${ECHO} "Removing rebuildable files..."	
	${RM} -f ${rebuildables}
	@${ECHO} "Done removing rebuildable files."	

define display-help
${MAKE} --print-data-base --question | \
${AWK} '/^[^.%][-A-Za-z0-9_]*:/ \
{ print substr($$1, 1, length($$1)-1) }' | \
${SORT} | \
${PR} --omit-pagination --width=80 --columns=4
endef

.PHONY: help
help:
	@${ECHO} "List of targets:"	
@${display-help}
