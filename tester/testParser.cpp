#include "../parser.hpp"
#include <cstdio>
#include <string>

int main(int argc, char const *argv[])
{
	string query_str("creAte tAble aa(name varchar(30), time intt PRIMARY KEY)");
	Parser *parser = new Parser(query_str);
    if(parser->Parse()){
    	parser->Print();
    }
    delete parser;

    query_str = "INSERT INTO STUDENT (a, b, c, d, e)\nVALUES(1, 1, 3, 'eqweqwe', '123213eqwd')";
	parser = new Parser(query_str);
    if(parser->Parse()){
    	parser->Print();
    }
    delete parser;
	return 0;
}
