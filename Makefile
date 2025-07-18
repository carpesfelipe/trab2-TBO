all:
	gcc -Wall -o trab2 *.c -lm
run:
	./trab2 2 4 1,0 0,2 examples/1/file1.txt
val:
	valgrind --leak-check=full -s ./trab2 2 10 1,0 0,2 examples/test/file1.txt
juncao_1:
	valgrind --leak-check=full -s ./trab2 2 2 0,1,4 4,0,1 examples/1/file1.txt examples/1/file2.txt out1.txt
juncao_2:
	valgrind --leak-check=full -s ./trab2 2 3 0,1,4 4,0,1 examples/2/file1.txt examples/2/file2.txt out2.txt
teste1:
	valgrind --leak-check=full -s ./trab2 2 7 0,2 0,1 testes/teste1/in1.txt testes/teste1/in2.txt testes/teste1/out.txt
teste2:
	valgrind --leak-check=full -s ./trab2 10 10 0,1,4 4,0,1 testes/teste2/in1.txt testes/teste2/in2.txt testes/teste2/out.txt
todos_testes:
	valgrind --leak-check=full -s ./trab2 2 2 0,1,4 4,0,1 examples/1/file1.txt examples/1/file2.txt saidas/out1.txt
	valgrind --leak-check=full -s ./trab2 2 3 0,1,4 4,0,1 examples/2/file1.txt examples/2/file2.txt saidas/out2.txt
	valgrind --leak-check=full -s ./trab2 3 7 1,0 0,2 examples/3/file1.txt examples/3/file2.txt saidas/out3.txt
	valgrind --leak-check=full -s ./trab2 2 7 0,2 0,1 examples/4/in1.txt examples/4/in2.txt saidas/out4.txt
	rm -f trab2 *.txt
diff:
	diff saidas/out1.txt examples/1/file.txt
	diff saidas/out2.txt examples/2/file.txt
	diff saidas/out3.txt examples/3/file.txt
clean:
	rm -f trab2 *.txt
zip:
TARNAME = trab2
FILES = *.c *.h Makefile

tarball:
	tar -czvf $(TARNAME).tar.gz $(FILES)
clean_saidas:
	rm -f trab2 saidas/*.txt

