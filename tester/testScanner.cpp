#include <cstdio>
#include <iostream>
#include <string>
#include "scanner.hpp"

using namespace std;
int main(int argc, char const *argv[])
{
	string query_str("The is a string.\nA_B_C,QQ123,(google.com)");
	Scanner scanner(query_str);
	string token;
	while ((token = scanner.nextToken()) != "") {
		cout << token << '\n';
	}
	return 0;
}