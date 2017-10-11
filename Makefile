GCC=gcc
CFLAGS = -c -Wall
OBJ = myshell.o test.o flex.o


shell: clean lex.yy.c lex.o myshell.o link.o test.o
	gcc lex.yy.c myshell.o test.o -o shell

lex.yy.c:
	flex lex.c 

lex.o:
	$(GCC) $(CFLAGS) lex.yy.c

myshell.o:
	$(GCC) $(CFLAGS) myshell.c

link.o:
	$(GCC) -lfl lex.yy.o myshell.o

test.o:
	$(GCC) $(CFLAGS) test.c


clean:
	rm -rf *o lex.yy.c 
