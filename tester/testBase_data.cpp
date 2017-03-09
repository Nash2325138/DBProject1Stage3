#include "../base_data.hpp"
#include "../parser.hpp"
#include "../color.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;

int main(int argc, char const *argv[])
{
	vector<string> queries;
    queries.push_back("creAte tAble aa(name varchar(30), time int PRIMARY KEY)");
    // queries.push_back("INSERT INTO STUDENT (a, b, c, d, e)\nVALUES(1, 1, 3, 'eqweqwe', '123213eqwd')");
    // queries.push_back("INSERT INTO ^ (a, b, c, d, e)\nVALUES(1, 1, 3, eqweqwe, '123213eqwd')");
    // queries.push_back("INSERT INTO STUDENT VALUES(1, 1, , 'eqweqwe', '')");
    queries.push_back("CREATE TABLE BB(name varchar(30), id varchar(20), a int)");
    // queries.push_back("CREATE TABLE aa(name varchar(30), a int)");
    queries.push_back("insert into aa Values('dandnas', 100)");
    queries.push_back("insert into aa values('qdwqdwqd', 20)");
    queries.push_back("insert into aa values('qdwqdwqd', 10)");
    queries.push_back("insert into aa values('diff', 20)");
    queries.push_back("insert into bb values('kk', 's10306XXXX', -30)");
    queries.push_back("cReate TAble qq(name varchar(10) PRIMARY KEY, a int, b int)");
    queries.push_back("insert Into qq values('1', '2')");
    queries.push_back("insert Into qq values('John', 2, 5)");
    queries.push_back("insert Into qq values('Susan', 20)");
    queries.push_back("insert Into qq values('Zoe', 2)");		// non-primary allow duplicate
    queries.push_back("insert Into qq values('Susan', 25)");
    queries.push_back("insert iNtO Qq (a, b, name) values(20, 30, 'Bob')");
    queries.push_back("insert iNtO Qq (a, name, b) values(,,1)");
    queries.push_back("insert iNtO Qq (name, b) values(,,1)");
    queries.push_back("insert iNtO Qq (name, b) values(,1)");
    queries.push_back("insert iNtO Qq (name, b) values('Jason',1)");
    queries.push_back("insert iNtO Qq (a, b) values(3,1)");


    BaseData base;
    for (auto& s : queries) {
    	printf(">> %s\n", s.c_str());
    	if (not base.Query(s)) {
    		// fprintf(stderr, "-> Query not legal\n");
    	}
    }
   	base.show();
   return 0;
}
