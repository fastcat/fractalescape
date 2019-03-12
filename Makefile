TARGET=fractalescape
SOURCES=escape.c main.c args.c random.c makefract.c \
	pngwriter.c brightscore.c varscore.c
OBJECTS=$(patsubst %.c,%.o,${SOURCES})
DEPS=$(patsubst %.c,%.d,${SOURCES})
CPPFLAGS=-D_GNU_SOURCE
CFLAGS=-Wall -Werror -g -O3
CC=gcc
LIBS=$(shell libpng-config --ldflags)
INSTBASE=${HOME}

all: ${TARGET}

dep: ${DEPS}

${TARGET}: ${OBJECTS}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJECTS} ${LIBS}

%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

%.d: %.c
	@${CC} ${CPPFLAGS} -MM $< | sed -e 's/^\(.*\)\.o[ :]*/\1.o \1.d : /' >$@

install: all
	install ${TARGET} ${INSTBASE}/bin/

clean:
	rm -f *.o

distclean: clean
	rm -f *.d
	rm -f ${TARGET}

-include ${DEPS}
