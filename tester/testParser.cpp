#include "../parser.hpp"
#include <cstdio>
#include <string>

int main(int argc, char const *argv[])
{
	string query_str("creAAte tAble aa(name varchar(-10), time intt PRIMARY KEY)");
	Parser parser(query_str);
    parser.Parse();
    parser.Print();
	return 0;
}
