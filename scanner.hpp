#include <cstdio>
#include <string>
#include <cstring>

using std::string;
class Scanner
{
private:
	char query[100000];
	int pos;
public:
	Scanner(string str);
	~Scanner();
	
	string nextToken();
	string lookAhead();
};