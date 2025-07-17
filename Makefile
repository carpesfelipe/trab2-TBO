all:
	gcc -Wall -o trab2 *.c -lm
run:
	./trab2 2 4 1,0 0,2 examples/1/file1.txt
val:
	valgrind --leak-check=full -s ./trab2 2 10 1,0 0,2 examples/test/file1.txt
juncao_1:
	valgrind --leak-check=full -s ./trab2 2 2 1,0 4,0,1 examples/test/file1.txt examples/test/file2.txt teste_juncao/1/arquivo.txt
juncao_2:
	valgrind --leak-check=full -s ./trab2 2 4 1,0 0,2 examples/test/file1.txt examples/test/file2.txt examples/test/out.txt
clean:
	rm -f trab2 *.txt