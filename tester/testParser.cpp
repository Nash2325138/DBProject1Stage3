#include "../parser.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;

int main(int argc, char const *argv[])
{
	Parser *parser;
    vector<string> queries;
    
    queries.push_back("creAte tAble aa(name varchar(30), time intt PRIMARY KEY)");
    queries.push_back("INSERT INTO STUDENT (a, b, c, d, e)\nVALUES(1, 1, 3, 'eqweqwe', '123213eqwd')");
    queries.push_back("INSERT INTO ^ (a, b, c, d, e)\nVALUES(1, 1, 3, eqweqwe, '123213eqwd')");
    queries.push_back("INSERT INTO STUDENT VALUES(1, 1, , 'eqweqwe', '')");
    queries.push_back("CREATE TABLE BB(name varchar(30), name varchar(20), a int)");
    queries.push_back("CREATE TABLE aa(name varchar(30), a int)");
    for (auto& s : queries) {
        parser = new Parser(s);
        if(parser->Parse()){
            parser->Print();
            puts("");
        }
        delete parser;     
    }
   return 0;
}
