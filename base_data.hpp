#include "parser.hpp"
#include "scanner.hpp"

class Table{
private:
	string table_name;
	vector<Attribute> schema;

public:
	Table(string table_name, vector<Attribute> schema): table_name(table_name), schema(schema);
	~Table(){};
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