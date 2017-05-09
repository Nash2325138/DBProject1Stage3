#ifndef BASE_DATA_HPP
#define BASE_DATA_HPP

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <fstream>
#include "parser.hpp"
#include "scanner.hpp"
#include "index_struct.hpp"
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
	void add_new_tuple(const vector<Value>& tuple, const Value& primary_key_value);
	bool checkDataType(string& attr_name, Value &value);
	bool checkPrimaryKey(Value &value);
	bool checkAttrNameExist(const string& attr_name);
	int findAttrColume(const string& attr_name);
	bool writeNewRecord(const vector<Value> &tuple);
	void readTuples();
	void reconstruct_index_structs();
	bool setIndex(const string& attr_name, const string& index_type);

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

	// map column to index_struct;
	map<int, Index_Struct*> index_structs;
	map<int, string> index_structs_meta_data;

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & table_name;
        ar & schema;
        ar & name_to_i;
        ar & hasPrimary;
        ar & primary_key_name;
        ar & primary_key_columns;
        ar & index_structs_meta_data;
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
	Parser::SelectQueryData *sData;
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

	bool select();
	vector<int>* get_filtered_rows_from_cmpps(Table* t);
	vector<int>* get_filtered_rows_from_cmpps(Table* t1, Table* t2, int t1_now_row);
	bool checkSelectQueryData();
	void fillOutputTableSchema(vector<pair<Table*, int> >& selectedAttributes);
	void fill_comparePairs_table_col();
	void reconstructTables();
	string getTrueTableName(string tableID);
	bool judgeComparePair(Value* v1, CompareOP op, Value* v2);
	bool judgeWhere(pair<Table*, int>& t1_row);
	bool judgeWhere(pair<Table*, int>& t1_row, pair<Table*, int>& t2_row);
	void push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row);
	void push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row, pair<Table*, int> t2_row);
	bool checkTableExistence(vector<string>& fromTables);
	bool checkAttributeStatus();
	bool checkPairTypes();
	string getTypeString(CompareType CT);
	string getAttributeType(AttributeID attrID);
	Table* getSourceTable(AttributeID& attrID);
	vector<pair<Table_col, Table_col> > comparePairs_table_col;
	void show();
	void save_tables_meta();
};

#endif