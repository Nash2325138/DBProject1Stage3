#include "scanner.hpp"
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>

using std::string;
Scanner::Scanner(string str): pos(0) {
	strcpy(query, str.c_str());
};
Scanner::~Scanner() {}
	
string Scanner::nextToken() {
	char buffer[10000];
	buffer[0] = '\0';
	int chars_read;
	if (sscanf(query + pos, " \"%[^\"]\"%n", buffer, &chars_read)) {
		char temp[10002];
		strcat(strcat(strcpy(temp, "\""), buffer), "\"");
		strcpy(buffer, temp);
		// printf("%s\n", buffer);
		// exit(EXIT_FAILURE);
	}
	else if (sscanf(query + pos, " \'%[^\']\'%n", buffer, &chars_read)) {
		char temp[10002];
		strcat(strcat(strcpy(temp, "\'"), buffer), "\'");
		strcpy(buffer, temp);
	}
	else if (sscanf(query + pos, " %[A-Za-z_0-9-]%n", buffer, &chars_read)){}
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9- \n]%n", buffer, &chars_read)){}
	else {
		fprintf(stderr, "sscanf wrong at nextToken()\n");
	}
	pos += chars_read;

	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	// if(ret[0] == '\"') {
	// 	std::cout << ret << '\n';
	// 	exit(EXIT_FAILURE);
	// }
	return ret;
}
string Scanner::lookAhead() {
	char buffer[1000];
	buffer[0] = '\0';

	if (sscanf(query + pos, " \"%[^\"]\"%n", buffer)) {
		char temp[10002];
		strcat(strcat(strcpy(temp, "\""), buffer), "\"");
		strcpy(buffer, temp);
	}
	else if (sscanf(query + pos, " \'%[^\']\'%n", buffer)) {
		char temp[10002];
		strcat(strcat(strcpy(temp, "\'"), buffer), "\'");
		strcpy(buffer, temp);
	}
	else if (sscanf(query + pos, " %[A-Za-z_0-9-]", buffer)){}
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9- \n]", buffer)){}
	else {
		fprintf(stderr, "sscanf wrong at lookAhead()\n");
	}
	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}
