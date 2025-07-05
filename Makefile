all:
	gcc -Wall -o trab2 *.c
run:
	./trab2 1 2 1,2,3 4,5,5
val:
	valgrind ./trab2 1 2 1,2,3 4,5,5
clean:
	rm -f trab2