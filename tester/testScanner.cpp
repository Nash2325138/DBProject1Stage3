#include <cstdio>
#include <iostream>
#include <string>
#include "../scanner.hpp"

using namespace std;
int main(int argc, char const *argv[])
{
	string query_str("A string.\nA_B_C,\'QQ\n 123,\' (-10), kk\nk-bb,\' ad LAla-\' !!~~ \"vk 123 Aaaaa\"");//, 'abg e^() wow'");
	Scanner scanner(query_str);
	string token;
	int i = 0;
	string lookahead = scanner.lookAhead();
	while ((token = scanner.nextToken()) != "" and i < 50) {
		cout << token << '\n';
		i++;
		if (token != lookahead) {
			fprintf(stderr, "token and lookahead not matched! ----> token:%s;lookahead:%s\n",
							 token.c_str(), lookahead.c_str());
		}
		lookahead = scanner.lookAhead();
	}
	return 0;
}
