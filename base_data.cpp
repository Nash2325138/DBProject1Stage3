#include "base_data.hpp"

Table::Table(string& table_name, vector<Attribute>& schema): table_name(table_name), schema(schema){}
Table::Table(){}
Table::~Table(){}
void Table::insert(vector<string>& orders, vector<Value>& values) {

}
void Table::insert(vector<Value>& values) {

}

bool BaseData::Query(string query_str){
	parser = new Parser(query_str);
	parser->Parse();

	if(parser->isCreateTableQuery){
		// Create table
		// Table = new Table
		// tables.pushback(table)
	}
	else if(parser->isInsertQuery){
		// tables.find()
		// put schema into table
		// tables[index].insert()
	}

	delete parser;
	return true;
}