scanner_test:
	g++ -Wall -std=c++11 scanner.hpp scanner.cpp tester/testScanner.cpp
	./a.out
	rm -f a.out

parser_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testParser.cpp
	./a.out
	rm -f a.out

base_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testBase_data.cpp
	./a.out
	rm -f a.out