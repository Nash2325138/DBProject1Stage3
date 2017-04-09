#include "../parser.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;

int main(int argc, char const *argv[])
{
	Parser *parser;
    vector<string> queries;
    setbuf(stdin, NULL);
    // printf("!\n");
    FILE* fp;
    if ((fp = fopen("sql/parser_testing.sql", "r")) == NULL) {
        perror("file read error");
        exit(1);
    }
    // printf("!\n");
    char buffer[1000];
    while (fgets(buffer, 1000, fp)) {
        // printf("?\n");
        buffer[strlen(buffer)-1] = '\0';
        // queries.emplace_back(buffer);
    }
    // printf("!\n");
    // queries.push_back("creAte tAble aa(name varchar(30), time intt PRIMARY KEY)");
    // queries.push_back("INSERT INTO STUDENT (a, b, c, d, e)\nVALUES(1, 1, 3, 'eqweqwe', '123213eqwd')");
    // queries.push_back("INSERT INTO ^ (a, b, c, d, e)\nVALUES(1, 1, 3, eqweqwe, '123213eqwd')");
    // queries.push_back("INSERT INTO STUDENT VALUES(1, 1, , 'eqweqwe', '')");
    // queries.push_back("CREATE TABLE BB(name varchar(30), name varchar(20), a int)");
    // queries.push_back("CREATE TABLE aa(name varchar(30), a int)");
    queries.push_back("SELECT Fname FROM names WHERE Fname = 'DBMS' OR        Lname = 'DBMSSSSSS' ");

    for (auto& s : queries) {
        parser = new Parser(s);
        cout << s << endl;
        if(parser->Parse()){
            parser->Print();
            puts("");
        } else {
            exit(1);
        }
        delete parser;     
    }
   return 0;
}
