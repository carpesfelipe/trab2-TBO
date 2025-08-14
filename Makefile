rall:
	gcc -Wall -o trab2 *.c -lm
run:
	./trab2 2 4 1,0 0,2 examples/1/file1.txt
val_pequenos:
	/usr/bin/time -v ./trab2 2 1000000 0 0 tests/in/pequenos/01/file1.txt tests/in/pequenos/01/file2.txt out_pequeno/sol1.txt
	systemd-run --user --scope -p MemoryMax=1M ./trab2 2 5000 1 1 tests/in/pequenos/02/file1.txt tests/in/pequenos/02/file2.txt out_pequeno/sol2.txt 
	systemd-run --user --scope -p MemoryMax=4M ./trab2 2 20000 2 2 tests/in/pequenos/03/file1.txt tests/in/pequenos/03/file2.txt out_pequeno/sol3.txt 
	rm -f *.txt
val_grande:
	time systemd-run --user --scope -p MemoryMax=70M ./trab2 4 100000 10,7 10,7 tests/in/grandes/01/file1.txt tests/in/grandes/01/file2.txt out_grande/sol1.txt
val_giga:
	time systemd-run --user --scope -p MemoryMax=7M ./trab2 10 10000 9,1 9,1 tests/in/gigantes/01/file1.txt tests/in/gigantes/01/file2.txt out_gigante/sol1.txt

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
	valgrind --leak-check=full -s ./trab2 10 12 0,1,4 4,0,1 examples/2/file1.txt examples/2/file2.txt saidas/out2.txt
	rm -f trab2 *.txt
diff:
	diff /home/felipecarpes/v3-tbo2/trab2-TBO/out_pequeno/sol1.txt /home/felipecarpes/v3-tbo2/trab2-TBO/tests/in/pequenos/01/sol.txt
	diff /home/felipecarpes/v3-tbo2/trab2-TBO/out_pequeno/sol2.txt /home/felipecarpes/v3-tbo2/trab2-TBO/tests/in/pequenos/02/sol.txt
	diff /home/felipecarpes/v3-tbo2/trab2-TBO/out_pequeno/sol3.txt /home/felipecarpes/v3-tbo2/trab2-TBO/tests/in/pequenos/03/sol.txt
	diff /home/felipecarpes/v3-tbo2/trab2-TBO/out_gigante/sol1.txt /home/felipecarpes/v3-tbo2/trab2-TBO/tests/in/gigantes/01/sol.txt
	diff /home/felipecarpes/v3-tbo2/trab2-TBO/out_grande/sol1.txt /home/felipecarpes/v3-tbo2/trab2-TBO/tests/in/grandes/01/sol.txt
clean:
	rm -f trab2 *.txt
zip:
TARNAME = 2022102425,2022101541
FILES = *.c *.h Makefile

tarball:
	tar -czvf $(TARNAME).tar.gz $(FILES)
clean_saidas:
	rm -f trab2 saidas/*.txt
