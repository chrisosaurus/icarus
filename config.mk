VERSION = 0.1

PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

INCS =
LIBS =

CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wdeclaration-after-statement -Wunused-function -Wmaybe-uninitialized ${INCS}

LDFLAGS = ${LIBS}

DEBUGFLAGS = -DDEBUG_ICARUS -DDEBUG_LEXER

CC = cc
