all:
	gcc -Wall -o main *.c
run:
	./main 1 2 1,2,3 4,5,5
val:
	valgrind ./main 1 2 1,2,3 4,5,5