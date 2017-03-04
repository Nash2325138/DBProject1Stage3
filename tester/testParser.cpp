#include "../parser.hpp"
#include <cstdio>
#include <string>

int main(int argc, char const *argv[])
{
	string query_str("creAte tAble aa(name varchar(-10), time int PRIMARY KEY)");
	Parser parser(query_str);
    parser.Parse();
    parser.Print();
	return 0;
}
