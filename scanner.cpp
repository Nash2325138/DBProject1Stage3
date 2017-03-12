#include "scanner.hpp"
#include "color.h"
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdarg>

using std::string;
Scanner::Scanner(const string& str): pos(0), aheadConsumed(true), aheadBuffer() {
	strcpy(query, str.c_str());
};
Scanner::~Scanner() {}
	
string Scanner::nextToken() {
	if (not aheadConsumed) {
		aheadConsumed = true;
		return aheadBuffer;
	}

	char buffer[10000];
	buffer[0] = '\0';
	int chars_read = 0;
	if (pos >= strlen(query)) {
		return "";
	}
	if (sscanf(query + pos, " %1[\"]%n", buffer, &chars_read)) {
		pos += chars_read;
		if (query[pos] == '\"') {
			// deal with empty string ""
			// printf("EMPTY_QUOTES\n");
			pos ++;
			return "\"\"";
		} else if (query[pos] == '\0') {
			// deal with a null charactor following the quotes ----> \"\0
			return "\"";
		}
		else {
			// deal with non-empty string
			sscanf(query + pos, "%[^\"]%n", buffer, &chars_read);
			char temp[10002];
			if (query[pos + chars_read] == '\"') { 
				// deal with string with close right quote
				strcat(strcat(strcpy(temp, "\""), buffer), "\"");
				strcpy(buffer, temp);
				chars_read++;
			} else {
				// deal with string with NO close right quote (btw it's illegal)
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
	else if (sscanf(query + pos, " %[A-Za-z_0-9-]%n", buffer, &chars_read)){} // deal with strings with no special sign 
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9 \n-]%n", buffer, &chars_read)){} // deal with strings with special sign (len == 1)
	else {
		fprintf(stderr, "sscanf wrong at nextToken()\n");
	}
	pos += chars_read;

	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}

string Scanner::lookAhead() {
	if (not aheadConsumed) {
		return aheadBuffer;
	}

	char buffer[10000];
	buffer[0] = '\0';

	if (pos >= strlen(query)) {
		return "";
	}

	int chars_read = 0;
	if (sscanf(query + pos, " %1[\"]%n", buffer, &chars_read)) {
		int temp_pos = pos + chars_read;
		if (query[temp_pos] == '\"') {
			aheadBuffer = "\"\"";
			return "\"\"";
		} else if (query[temp_pos] == '\0') {
			aheadBuffer = "\"";
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
			aheadBuffer = "\'\'";
			return "\'\'";
		} else if (query[temp_pos] == '\0') {
			aheadBuffer = "\'";
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
	else if (sscanf(query + pos, " %1[^A-Za-z_0-9 \n-]", buffer)){}
	else {
		fprintf(stderr, "sscanf wrong at lookAhead()\n");
	}
	string ret(buffer);
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	aheadBuffer = ret;
	return ret;
}

void printErr(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char * buffer = new char[strlen(fmt)+100];
    strcat(strcat(strcat(buffer, LIGHT_RED), fmt), NONE);
    vfprintf(stderr, buffer, args);
    va_end(args);
}
