#ifndef BASE_DATA_HPP
#define BASE_DATA_HPP

#include "parser.hpp"
#include "scanner.hpp"
#include <map>
#include <set>

using namespace std; 
class Table{
public:
	Table(string& table_name, vector<Attribute>& schema);
	Table();
	~Table();
	bool insert(vector<string>& orders, vector<Value>& values);
	bool insert(vector<Value>& values);
	bool checkDataType(string& attr_name, Value &value);
	bool checkPrimaryKey(Value &value);
	const vector<int>& matchedAttributes(const string str);

	void show();
	string schemaToString();

	string table_name;
	vector<Attribute> schema;

	map<string, int> name_to_i;
	vector<vector<Value> > tuples; // map from attribute name to a Value
	
	bool hasPrimary;
	string primary_key_name;
	set<Value> primary_key_columns;
};

class OutputTable {
public:
	vector<string> schema;
	map<string, int> name_to_i;
	vector<vector<Value> > tuples;
	void show();
};

class BaseData{
private:
	map<string, Table> tables;
	Parser *parser;
	OutputTable outputTable;
public:
	BaseData(){};
	~BaseData(){};
	bool Query(string query_str);

	bool select(Parser::SelectQueryData& sData);
	bool checkSelectQueryData(Parser::SelectQueryData& sData);
	void fillOutputTableSchema(Parser::SelectQueryData& sData, 
							   vector<pair<string, int>>& selectedAttributes);
	string getTrueTableName(Parser::SelectQueryData& sData, string tableID);
	bool checkTableExistence(vector<string>& fromTables);
	bool checkAttributeStatus(Parser::SelectQueryData& selectedData);
	bool checkPairTypes(Parser::SelectQueryData& selectedData);
	string getTypeString(CompareType CT);
	string getAttributeType(AttributeID attrID, Parser::SelectQueryData& selectedData);

	void show();
};

#endif