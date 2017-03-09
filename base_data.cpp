#include <iostream>
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

bool Table::checkPrimaryKey(Value &value){
	if(!primary_key_columns.empty() && (primary_key_columns.find(value) != primary_key_columns.end())){
		fprintf(stderr, LIGHT_RED "Duplicated primary key value\n" WHITE);
		return false;
	}
	return true;
}

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
		if(not checkDataType(orders[i], values[i])){
			return false;
		}
		if(hasPrimary){
			if(orders[i] == primary_key_name){
				if(not checkPrimaryKey(values[i])){
					return false;
				}
				primary_key_columns.insert(values[i]);
			}
		}
		tuple[orders[i]] = values[i];
	}
	tuples.push_back(tuple);
	return true;
}

bool Table::insert(vector<Value>& values) {
	map<string, Value> tuple;
	for(int i=0; i<values.size(); i++){
		// check data type
		if(not checkDataType(schema[i].name, values[i])){
			return false;
		}
		if(hasPrimary){
			if(schema[i].isPrimaryKey){
				if(not checkPrimaryKey(values[i])){
					return false;
				}
				primary_key_columns.insert(values[i]);
			}
		}	
		tuple[schema[i].name] = values[i];
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

void Table::show(){
	auto max = [](int x, int y) -> int {return (x > y) ? x : y;};
	static const int OFFSET = 2;
	int i=0;

	// compute column widths to output
	vector<int> column_widths(schema.size());
	for (int i=0; i<schema.size(); i++) {
		column_widths[i] = schema[i].name.length();
	}
	for (auto& tuple : tuples){
		i=0;
		for (auto& pair : tuple){
			column_widths[i] = max(column_widths[i], pair.second.toString().length());
			i++;
		}
	}
	for (int& i:column_widths) {
		i += OFFSET;
	}

	// ouput all 
	printf("Table: %s\n", table_name.c_str());
	i=0;
	for (auto& attr : schema) {
		printf("|%*s", column_widths[i], attr.name.c_str());
		i++;
	}
	puts("|");

	for (auto& tuple : tuples){
		i=0;
		for (auto& attr : schema){
			printf("|%*s", column_widths[i], tuple[attr.name].toString().c_str());
			i++;
		}
		puts("|");
	}
	puts("");
}
void BaseData::show() {
	std::cout << "Base: " << std::endl;
	for (auto& p: tables) {
		auto& table = p.second;
		table.show();
	}
	puts("");
}