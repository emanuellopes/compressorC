# Easily adaptable makefile for Advanced Programming
# Note: remove comments (#) to activate some features
#
# author Vitor Carreira
# date 2010-09-26


# Libraries to include (if any)
LIBS=-lm -pthread libpacodec_x64.a

# Compiler flags
CFLAGS=-Wall -W -Wmissing-prototypes #-m32
LDFLAGS=-m32

# Indentation flags
IFLAGS=-br -brs -npsl -ce -cli4


# Name of the executable
PROGRAM=pacodec

# Prefix for the gengetopt file (if gengetopt is used)
PROGRAM_OPT=options

# Object files required to build the executable
PROGRAM_OBJS=main.o debug.o memory.o ${PROGRAM_OPT}.o encode.o signal_handler.o

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: ${PROGRAM}

# compilar com depuracao
debugon: CFLAGS += -D SHOW_DEBUG -g
debugon: ${PROGRAM}

${PROGRAM}: ${PROGRAM_OBJS}
	${CC} -o $@ ${PROGRAM_OBJS} ${LIBS}

# Dependencies
main.o: main.c debug.h memory.h errors.h ${PROGRAM_OPT}.h encode.h signal_handler.h
${PROGRAM_OPT}.o: ${PROGRAM_OPT}.c ${PROGRAM_OPT}.h

debug.o: debug.c debug.h
memory.o: memory.c memory.h
encode.o: encode.c encode.h
signal_handler.o: signal_handler.c signal_handler.h

#how to create an object file (.o) from C file (.c)
.c.o:
	${CC} ${CFLAGS} -c $<

# Generates command line arguments code from gengetopt configuration file
${PROGRAM_OPT}.h: ${PROGRAM_OPT}.ggo
	gengetopt < ${PROGRAM_OPT}.ggo --file-name=${PROGRAM_OPT}

clean:
	rm -f *.o core.* *~ ${PROGRAM} *.bak ${PROGRAM_OPT}.h ${PROGRAM_OPT}.c

docs: Doxyfile
	doxygen Doxyfile

Doxyfile:
	doxygen -g Doxyfile

indent:
	indent ${IFLAGS} *.c *.h
