bison -d calc.y
mv calc.tab.h calc.h
mv calc.tab.c calc.y.c
flex calc.l
mv lex.yy.c calc.l.c
gcc -c calc.l.c -o calc.l.o
gcc -c calc.y.c -o calc.y.o
gcc -o calc calc.l.o calc.y.o -ll -lm -lfl