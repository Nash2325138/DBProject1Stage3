all:
	g++ -std=c++11 *.cpp *.hpp

scanner_test:
	g++ -Wall -std=c++11 scanner.hpp scanner.cpp tester/testScanner.cpp
	./a.out

parser_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testParser.cpp
	./a.out

base_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testBase_data.cpp
	./a.out

run_a_lot:
	for i in {1..100}; do ./a.out 2>&1 >/dev/null ; done