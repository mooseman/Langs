
#  toySQL - Makefile 
#  Based on the makefile for the Lice compiler which was 
#  written by Dale Weiler 


CC      ?= gcc
CFLAGS  += -Wall -Wextra -Wno-missing-field-initializers -O3 -std=c99 
LDFLAGS +=

SQLSOURCES = ast.c parse.c lice.c gen.c gen_amd64.c lexer.c util.c conv.c decl.c init.c list.c opt.c
ARGSSOURCES = misc/argsgen.c util.c list.c
TESTSOURCES = test.c util.c list.c
SQLOBJECTS = $(SQLSOURCES:.c=.o)
TESTOBJECTS = $(TESTSOURCES:.c=.o)
ARGSOBJECTS = $(ARGSSOURCES:.c=.o)
SQLDEPENDS  = $(SQLSOURCES:.c=.d)
TESTDEPENDS = $(TESTSOURCES:.c=.d)
ARGSDEPENDS = $(ARGSSOURCES:.c=.d)
SQLBIN      = toysql
TESTBIN     = testsuite
ARGSBIN     = argsgenerator

all: $(SQLBIN) $(TESTBIN)

$(SQLBIN): $(SQLOBJECTS)
	$(CC) $(LDFLAGS) $(SQLOBJECTS) -o $@

$(TESTBIN): $(TESTOBJECTS)
	$(CC) $(LDFLAGS) $(TESTOBJECTS) -o $@

$(ARGSBIN): $(ARGSOBJECTS)
	$(CC) $(LDFLAGS) $(ARGSOBJECTS) -o $@

c.o:
	$(CC) -c $(CFLAGS) $< -o $@

args: $(ARGSBIN)
	@./$(ARGSBIN)

clean:
	rm -f $(SQLOBJECTS)
	rm -f $(TESTOBJECTS)
	rm -f $(SQLDEPENDS)
	rm -f $(TESTDEPENDS)
	rm -f $(ARGSDEPENDS)
	rm -f $(SQLBIN)
	rm -f $(TESTBIN)
	rm -f $(ARGSBIN)

test: $(SQLBIN) $(TESTBIN)
	@./$(TESTBIN)



