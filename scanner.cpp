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
	int chars_read = 0;
	if (pos >= strlen(query)) {
		return "";
	}
	if (sscanf(query + pos, " %1[\"]%n", buffer, &chars_read)) {
		pos += chars_read;
		if (query[pos] == '\"') {
			// printf("EMPTY_QUOTES\n");
			pos ++;
			return "\"\"";
		} else if (query[pos] == '\0') {
			return "\"";
		}
		else {
			sscanf(query + pos, "%[^\"]%n", buffer, &chars_read);
			char temp[10002];
			if (query[pos + chars_read] == '\"') { 
				strcat(strcat(strcpy(temp, "\""), buffer), "\"");
				strcpy(buffer, temp);
				chars_read++;
			} else {
				strcat(strcpy(temp, "\""), buffer);
				strcpy(buffer, temp);
			}
		}
	} 
	else if (sscanf(query + pos, " %1[\']%n", buffer, &chars_read)) {
		pos += chars_read;
		if (query[pos] == '\'') {
			// printf("EMPTY_QUOTES\n");
			pos ++;
			return "\'\'";
		} else if (query[pos] == '\0') {
			return "\'";
		}
		else {
			sscanf(query + pos, "%[^\']%n", buffer, &chars_read);
			char temp[10002];
			if (query[pos + chars_read] == '\'') { 
				strcat(strcat(strcpy(temp, "\'"), buffer), "\'");
				strcpy(buffer, temp);
				chars_read++;
			} else {
				strcat(strcpy(temp, "\'"), buffer);
				strcpy(buffer, temp);
			}
		}
	}
	else if (sscanf(query + pos, " %[A-Za-z_0-9-]%n", buffer, &chars_read)){}
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9- \n]%n", buffer, &chars_read)){}
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

	if (pos >= strlen(query)) {
		return "";
	}

	int chars_read = 0;
	if (sscanf(query + pos, " %1[\"]%n", buffer, &chars_read)) {
		int temp_pos = pos + chars_read;
		if (query[temp_pos] == '\"') {
			return "\"\"";
		} else if (query[temp_pos] == '\0') {
			return "\"";
		}
		else {
			sscanf(query + temp_pos, "%[^\"]%n", buffer, &chars_read);
			char temp[10002];
			if (query[temp_pos + chars_read] == '\"') { 
				strcat(strcat(strcpy(temp, "\""), buffer), "\"");
				strcpy(buffer, temp);
				chars_read++;
			} else {
				strcat(strcpy(temp, "\""), buffer);
				strcpy(buffer, temp);
			}
		}
	} 
	else if (sscanf(query + pos, " %1[\']%n", buffer, &chars_read)) {
		int temp_pos = pos + chars_read;
		if (query[temp_pos] == '\'') {
			return "\'\'";
		} else if (query[temp_pos] == '\0') {
			return "\'";
		}
		else {
			sscanf(query + temp_pos, "%[^\']%n", buffer, &chars_read);
			char temp[10002];
			if (query[temp_pos + chars_read] == '\'') { 
				strcat(strcat(strcpy(temp, "\'"), buffer), "\'");
				strcpy(buffer, temp);
				chars_read++;
			} else {
				strcat(strcpy(temp, "\'"), buffer);
				strcpy(buffer, temp);
			}
		}
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
