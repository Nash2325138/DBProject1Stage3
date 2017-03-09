#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <cstdio>
#include <string>
#include <cstring>

using std::string;
class Scanner
{
private:
	char query[100000];
	int pos;

	bool aheadConsumed;
	string aheadBuffer;
public:
	Scanner(const string& str);
	~Scanner();
	
	string nextToken();
	string lookAhead();
};

void printErr(const char *fmt, ...);
#endif