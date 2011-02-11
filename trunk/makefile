CC=gcc
CFLAGS=-c -Wall -I.
LDFLAGS=
LHCC_SRCS= \
	arena.c \
	ast.c \
	atom.c \
	cfold.c \
	clexer.c \
	cparser.c \
	error.c \
	hcc.c \
	ssc-declr.c \
	ssc-exp.c \
	ssc-stmt.c \
	symbol.c \
	type.c \
	preprocessor/arith.c \
	preprocessor/assert.c \
	preprocessor/atest.c \
	preprocessor/cpp.c \
	preprocessor/eval.c \
	preprocessor/hash.c \
	preprocessor/lexer.c \
	preprocessor/macro.c \
	preprocessor/mem.c \
	preprocessor/nhash.c \
	preprocessor/sample.c \

LHCC_OBJECTS=$(LHCC_SRCS:.c=.o)
EXECUTABLE=hcc

all: $(LHCC_SRCS) $(EXECUTABLE)

$(EXECUTABLE): $(LHCC_OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@