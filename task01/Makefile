CC=gcc
IMPROVED=improved
target=plain $(IMPROVED) retval affix retsuff retinf retinfbak1 retinfbak2 retinfbak3
.c.o:
	echo rebuilding $*.o from $*.c
	$(CC) -ggdb -c $<

all: $(target)

plain: lex.o plain.o main.o 
	$(CC) -o ./plain plain.o lex.o main.o

retval: lex.o retval.o main.o  name.o
	$(CC) -o ./$@ $^

${IMPROVED}: lex.o ${IMPROVED}.o main.o name.o 
	$(CC) -o ./${IMPROVED} ${IMPROVED}.o lex.o main.o  name.o


affix: lex.o affix.o main.o 
	$(CC) -o ./affix name.o affix.o lex.o main.o

retsuff: lex.o retsuff.o main.o 
	$(CC) -o ./retsuff name.o retsuff.o lex.o main.o

retinf: lex.o retinf.o main.o 
	$(CC) -o ./retinf name.o retinf.o lex.o main.o

retinfbak1: lex.o retinfbak1.o main.o
	$(CC) -o ./retinfbak1 name.o retinfbak1.o lex.o main.o

retinfbak2: lex.o retinfbak2.o main.o
	$(CC) -o ./retinfbak2 name.o retinfbak2.o lex.o main.o


retinfbak3: lex.o retinfbak3.o main.o
	$(CC) -o ./retinfbak3 name.o retinfbak3.o lex.o main.o

lex.o: lex.h
	$(CC) -c lex.c

clean: 
	rm -rf *.o $(target)

# DO NOT DELETE

