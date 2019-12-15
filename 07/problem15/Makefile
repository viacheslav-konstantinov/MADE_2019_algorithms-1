CC=g++

all: TSPsolver singleNtest multipleNtest

runTSPsolver: TSPsolver
	./TSPsolver

runSingleNtest: singleNtest
	./singleNtest

runMultipleNtest: multipleNtest
	./multipleNtest

testTSPsolver: TSPsolver
	./TSPsolver < input.txt

testSingleNtest: singleNtest
	./singleNtest < input.txt

testMultipleNtest: multipleNtest
	./multipleNtest < input.txt

TSPsolver: main.o
	$(CC) -o TSPsolver main.o

main.o: main.cpp
	$(CC) -c main.cpp boxMuellerGenerator.hpp primMST.hpp accuracyScore.hpp

singleNtest: singleNtest.o
	$(CC) -o singleNtest singleNtest.o

singleNtest.o: singleNtest.cpp
	$(CC) -c singleNtest.cpp boxMuellerGenerator.hpp primMST.hpp accuracyScore.hpp

multipleNtest: multipleNtest.o
	$(CC) -o multipleNtest multipleNtest.o

multipleNtest.o: multipleNtest.cpp
	$(CC) -c multipleNtest.cpp boxMuellerGenerator.hpp primMST.hpp accuracyScore.hpp

clean:
	rm -rf *.o *.gch TSPsolver singleNtest multipleNtest