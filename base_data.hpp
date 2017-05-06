#ifndef BASE_DATA_HPP
#define BASE_DATA_HPP

#include "parser.hpp"
#include "scanner.hpp"
#include <map>
#include <set>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

typedef vector<Value> Tuple;
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
	vector<vector<Value> > tuples;
	void show();
	void clear() {
		schema.clear();
		tuples.clear();
	}

};

typedef pair<Table*, int> Table_col;
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
							   vector<pair<Table*, int> >& selectedAttributes);
	string getTrueTableName(Parser::SelectQueryData& sData, string tableID);
	bool judgeComparePair(Value* v1, CompareOP op, Value* v2);
	bool judgeWhere(Parser::SelectQueryData& sData, pair<Table*, int>& t1_row);
	bool judgeWhere(Parser::SelectQueryData& sData, pair<Table*, int>& t1_row, pair<Table*, int>& t2_row);
	void push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row);
	void push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row, pair<Table*, int> t2_row);
	bool checkTableExistence(vector<string>& fromTables);
	bool checkAttributeStatus(Parser::SelectQueryData& selectedData);
	bool checkPairTypes(Parser::SelectQueryData& selectedData);
	string getTypeString(CompareType CT);
	string getAttributeType(AttributeID attrID);
	Table* getSourceTable(AttributeID& attrID);
	vector<pair<Table_col, Table_col> > comparePairs_table_col;
	void show();
};

#endif