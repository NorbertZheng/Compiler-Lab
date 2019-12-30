CC = gcc
COMOBJ = y.tab.o lex.yy.o emalloc.o tree.o type.o

# ------------------------------------------------------------
all: lambda

clean: 
	rm *.o y.tab.c y.tab.h lex.yy.c

clpdf:
	rm *.output *.out *.pdf *.aux *.tex *.log

# ------------------------------------------------------------
.c.o:
	$(CC) -pg -g3 -Wall $(INCLUDE) -c $(CFLAGS) $(MODEL) $*.c

#------------------------------------------------------------
lex.yy.c: lexer.l
	flex  lexer.l

y.tab.c y.tab.h: grammar.y
	byacc -tvd grammar.y

#------------------------------------------------------------
lambda: $(COMOBJ)
	$(CC) -pg -g3 -o lambda $(LIB) $(MODEL) $(COMOBJ) $(CLIB)

#------------------------------------------------------------
emalloc.o: emalloc.c
lex.yy.o: lex.yy.c tree.h y.tab.h
tree.o: tree.c tree.h
type.o: type.c type.h tree.h tree.c
y.tab.o: y.tab.c tree.h


