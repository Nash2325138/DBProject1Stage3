CC=g++
FLAG=-Wall -std=c++11
SERIAL_FLAG=-lboost_serialization
OUTPUT_FILE=a.out
all: base_data.o main.cpp
	$(CC) $(FLAG) $(SERIAL_FLAG) main.cpp *.o -o $(OUTPUT_FILE)
	rm -f base.save
	rm -rf datas/
	mkdir datas

scanner_test: scanner.o
	$(CC) $(FLAG) *.o tester/testScanner.cpp
	./a.out

parser_test: parser.o
	$(CC) $(FLAG) *.o tester/testParser.cpp
	./a.out

base_test: base_data.o
	$(CC) $(FLAG) *.o tester/testBase_data.cpp
	./a.out

ex2_test: all
	rm -f base.save
	./$(OUTPUT_FILE) sql/ex2.sql > myex2.ans
	diff myex2.ans ex2.ans --side-by-side --suppress-common-lines
	./$(OUTPUT_FILE) sql/ex2_select.sql > myex2_select.ans
	diff myex2_select.ans ex2_select.ans --side-by-side --suppress-common-lines
	rm myex2.ans myex2_select.ans

loadind_test:
	./$(OUTPUT_FILE) test/trans.sql test/user.sql > /dev/null

total_test:
	$(CC) $(FLAG) $(SERIAL_FLAG) main.cpp *.o -o $(OUTPUT_FILE)
	rm -f base.save
	rm -rf datas/
	mkdir datas
	./$(OUTPUT_FILE) test/trans.sql test/user.sql > /dev/null
	./$(OUTPUT_FILE) test/setIndex.sql
	./$(OUTPUT_FILE) test/query1.sql
	./$(OUTPUT_FILE) test/query2.sql
	./$(OUTPUT_FILE) test/query3.sql
	./$(OUTPUT_FILE) test/query4.sql
	./$(OUTPUT_FILE) test/query5.sql

scanner.o: scanner.cpp scanner.hpp
	$(CC) $(FLAG) -c scanner.hpp scanner.cpp

parser.o: scanner.o parser.cpp parser.hpp
	$(CC) $(FLAG) -c parser.hpp parser.cpp

index_struct.o: parser.o index_struct.hpp index_struct.cpp
	$(CC) $(FLAG) -c index_struct.hpp index_struct.cpp

base_data.o: parser.o base_data.cpp base_data.hpp index_struct.o
	$(CC) $(FLAG) -c base_data.hpp base_data.cpp

clean:
	rm -f *.o *.out base.save $(OUTPUT_FILE)
