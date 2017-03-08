#include "base_data.hpp"
#include "color.h"

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
		Table table(parser->table_name, parser->schema);
		if(tables.find(table.table_name) != tables.end()){ // Duplicated table name
			fprintf(stderr, LIGHT_RED "Duplicated table name '%s'\n" WHITE, table.table_name.c_str());
			return false;
		}
		else {
			// Map table into tables
			tables[table.table_name] = table;
		}
	}
	else if(parser->isInsertQuery){
		// find the table
		if(tables.find(parser->table_name) == tables.end()){
			fprintf(stderr, LIGHT_RED "No such table '%s'\n" WHITE, parser->table_name.c_str());
			return false;
		}
		else {
			// put schema into table
			if(parser->orderSpecified){
				tables[parser->table_name].insert(parser->orders, parser->values);
			}
			else {
				tables[parser->table_name].insert(parser->values);
			}
		}
	}

	delete parser;
	return true;
}