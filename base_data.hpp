#ifndef BASE_DATA_HPP
#define BASE_DATA_HPP

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include "parser.hpp"
#include "scanner.hpp"
#include <map>
#include <set>

typedef vector<Value> Tuple;
using namespace std; 
class Table{
public:
	// class functions
	Table(string& table_name, vector<Attribute>& schema);
	Table();
	~Table();
	bool insert(vector<string>& orders, vector<Value>& values);
	bool insert(vector<Value>& values);
	bool checkDataType(string& attr_name, Value &value);
	bool checkPrimaryKey(Value &value);
	bool writeNewRecord(const string &table_name, const int &row);
	
	const vector<int>& matchedAttributes(const string str);

	void show();
	string schemaToString();

	// class members
	string table_name;
	vector<Attribute> schema;

	map<string, int> name_to_i;
	vector<vector<Value> > tuples; // map from attribute name to a Value
	
	bool hasPrimary;
	string primary_key_name;
	set<Value> primary_key_columns;

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & table_name;
        ar & schema;
        ar & name_to_i;
        ar & tuples;
        ar & hasPrimary;
        ar & primary_key_name;
        ar & primary_key_columns;
    }
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
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
    {
        ar & tables;
    }
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