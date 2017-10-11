GCC=gcc
CFLAGS = -c
OBJ = myshell.o test.o flex.o


shell: lex.yy.c lex.o link.o test.o
	gcc lex.yy.c myshell.o test.o -o shell

lex.yy.c:
	flex lex.c 

lex.o:
	$(GCC) $(CFLAGS) lex.yy.c

myshell.o:
	$(GCC) $(CFLAGS) myshell.c

link.o:
	$(GCC) -lfl lex.o myshell.o

test.o:
	$(GCC) $(CFLAGS) test.c


clean:
	rm -rf *o lex.yy.c 
