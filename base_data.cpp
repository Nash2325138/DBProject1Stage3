#include "base_data.hpp"
#include "color.h"

Table::Table(string& table_name, vector<Attribute>& schema): table_name(table_name), schema(schema)
															, hasPrimary(false){
	for (auto& attr : schema) {
		if (attr.isPrimaryKey) {
			hasPrimary = true;
			primary_key_name = attr.name;
			return;
		}
	}
}
Table::Table(){}
Table::~Table(){}

bool Table::checkDataType(string attr_name, Value &value){
	for(auto &attr: schema){
		if(attr.name == attr_name){
			if((attr.type == "int" && value.isInt) || (attr.type == "varchar" && value.isString)){ // correct type
				return true;
			}
			else { // incorrect type
				fprintf(stderr, LIGHT_RED "Type error: attribute '%s' should have '%s' type\n" WHITE, attr.name.c_str(), attr.type.c_str());
				return false;
			}
		}
	}
	fprintf(stderr, LIGHT_RED "No such attribute '%s'\n" WHITE, attr_name.c_str());
	return false;
}

bool Table::insert(vector<string>& orders, vector<Value>& values) {
	map<string, Value> tuple;
	for(int i=0; i<orders.size(); i++){
		// check data type
		if(checkDataType(orders[i], values[i])){
			tuple[orders[i]] = values[i];
		}
		else {
			return false;
		}
	}
	tuples.push_back(tuple);
	return true;
}

bool Table::insert(vector<Value>& values) {
	map<string, Value> tuple;
	for(int i=0; i<values.size(); i++){
		// check data type
		if(checkDataType(schema[i].name, values[i])){
			tuple[schema[i].name] = values[i];
		}
		else {
			return false;
		}
	}
	tuples.push_back(tuple);
	return true;
}

bool BaseData::Query(string query_str){
	parser = new Parser(query_str);
	if(not parser->Parse()) return false;

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