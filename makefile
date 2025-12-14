#
# Makefile
#

main: main.o
	gcc -o main main.o -lncurses

main.o: main.c

clean:
	rm -f '#*#'
	rm -f *.o
	rm -f *~
