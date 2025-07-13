all:
	gcc -Wall -o trab2 *.c
run:
	./trab2 2 4 1,0 0,2 examples/1/file1.txt
val:
	valgrind --leak-check=full -s ./trab2 2 4 1,0 0,2 examples/test/file1.txt
clean:
	rm -f trab2