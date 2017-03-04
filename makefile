scanner_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testScanner.cpp
	./a.out
	rm -f a.out

parser_test:
	g++ -Wall -std=c++11 *.hpp *.cpp tester/testParser.cpp
	./a.out
	rm -f a.out
