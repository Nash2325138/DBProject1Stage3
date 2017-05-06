CC=g++
FLAG=-Wall -std=c++11 -lboost_serialization
all: base_data.o
	$(CC) $(FLAG) main.cpp *.o
	rm -f base.save

scanner_test: scanner.o
	$(CC) $(FLAG) *.o tester/testScanner.cpp
	./a.out

parser_test: parser.o
	$(CC) $(FLAG) *.o tester/testParser.cpp
	./a.out

base_test: base_data.o
	$(CC) $(FLAG) *.o tester/testBase_data.cpp
	./a.out

scanner.o: scanner.cpp scanner.hpp
	$(CC) $(FLAG) -c scanner.hpp scanner.cpp

parser.o: scanner.o parser.cpp parser.hpp
	$(CC) $(FLAG) -c parser.hpp parser.cpp

base_data.o: parser.o base_data.cpp base_data.hpp
	$(CC) $(FLAG) -c base_data.hpp base_data.cpp

clean:
	rm -f *.o *.out base.save
