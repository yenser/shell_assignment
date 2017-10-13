GCC=gcc
CFLAGS = -c -Wall
OBJ = myshell.o test.o flex.o


shell: clean lex.yy.c
	gcc -o myshell lex.yy.c myshell.c -lfl

lex.yy.c:
	flex lex.c 

clean:
	rm -rf *o lex.yy.c myshell 
