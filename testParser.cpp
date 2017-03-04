#include "parser.hpp"
#include <cstdio>
#include <string>

int main(int argc, char const *argv[])
{
	string query_str("create table t1()");
	Parser parser(query_str);
	return 0;
}