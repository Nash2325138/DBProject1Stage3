#include "scanner.hpp"
#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>

using std::string;
Scanner::Scanner(string str): pos(0) {
	strcpy(query, str.c_str());
};
Scanner::~Scanner() {}
	
string Scanner::nextToken() {
	char buffer[1000];
	buffer[0] = '\0';
	int chars_read;
	if (sscanf(query + pos, " %[A-Za-z_0-9]%n", buffer, &chars_read)){}
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9 \n]%n", buffer, &chars_read)){}
	else {
		fprintf(stderr, "sscanf wrong at nextToken()\n");
	}
	pos += chars_read;

	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}
string Scanner::lookAhead() {
	char buffer[1000];
	buffer[0] = '\0';
	if (sscanf(query + pos, " %[A-Za-z_0-9]", buffer)){}
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9 \n]", buffer)){}
	else {
		fprintf(stderr, "sscanf wrong at lookAhead()\n");
	}
	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}
