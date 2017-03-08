#ifndef BASE_DATA_HPP
#define BASE_DATA_HPP

#include "parser.hpp"
#include "scanner.hpp"
#include <map>

using namespace std; 
class Table{
public:
	Table(string& table_name, vector<Attribute>& schema);
	Table();
	~Table();
	void insert(vector<string>& orders, vector<Value>& values);
	void insert(vector<Value>& values);

	string table_name;
	vector<Attribute> schema;
};

class BaseData{
private:
	map<string, Table> tables;
	Parser *parser;

public:
	BaseData(){};
	~BaseData(){};
	bool Query(string query_str);
};

#endif