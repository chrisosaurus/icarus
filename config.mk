VERSION = 0.1

PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

INCS =
LIBS =

# NB: including  -fprofile-arcs -ftest-coverage for gcov
# travis wasn't happy with -Wmaybe-uninitialized  so removed for now
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wdeclaration-after-statement -Wunused-function -fprofile-arcs -ftest-coverage ${INCS}

# gcov free version
#CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wdeclaration-after-statement -Wunused-function -Wmaybe-uninitialized ${INCS}

# NB: including  -fprofile-arcs for gcov
LDFLAGS = -fprofile-arcs ${LIBS}

# gcov free version
#LDFLAGS = ${LIBS}

#DEBUGFLAGS = -DDEBUG_ICARUS -DDEBUG_LEXER -DDEBUG_PARSE -DDEBUG_PARSE_EXPR
DEBUGFLAGS =

CC = cc
