VERSION = 0.1

PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

INCS =
LIBS =

# NB: including  -fprofile-arcs -ftest-coverage for gcov
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wdeclaration-after-statement -Wunused-function -Wmaybe-uninitialized -fprofile-arcs -ftest-coverage ${INCS}

# NB: including  -fprofile-arcs for gcov
LDFLAGS = -fprofile-arcs ${LIBS}

DEBUGFLAGS = -DDEBUG_ICARUS -DDEBUG_LEXER -DDEBUG_PARSE

CC = cc
