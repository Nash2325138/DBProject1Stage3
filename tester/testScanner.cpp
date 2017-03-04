#include <cstdio>
#include <iostream>
#include <string>
#include "../scanner.hpp"

using namespace std;
int main(int argc, char const *argv[])
{
	string query_str("The is a string.\nA_B_C,QQ123,(google.com), (-10), kkk-bb, \"LAla- !!~~ k 123\" Aaaaa");//, 'abg e^() wow'");
	Scanner scanner(query_str);
	string token;
	int i = 0;
	while ((token = scanner.nextToken()) != "" and i < 50) {
		// printf("%s\n", token.c_str());
		cout << token << '\n';
		i++;
	}
	return 0;
}