#include <iostream>
#include <algorithm>
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
	if (value.isNull) {
		printErr("Value of primary key can't be null\n");
		return false;
	}
	if(!primary_key_columns.empty() && (primary_key_columns.find(value) != primary_key_columns.end())){
		printErr("Duplicated primary key value\n");
		return false;
	}
	return true;
}

bool Table::checkDataType(string& attr_name, Value &value){
	for(auto &attr: schema){
		if(attr.name == attr_name){
			if (value.isNull) {
				// if it's primary key, it shouldn't be null, but the check is left for checkPrimayKey()
				return true;
			}
			else if (attr.type == "int" && value.isInt) {
				return true;
			}
			else if (attr.type == "varchar" && value.isString) {
				if (value.strData.length() > attr.char_len) {
					printErr("String length of attribut '%s' can not exceed %d\n", attr.name.c_str(), attr.char_len);
					return false;
				}
				return true;
			}
			else { // incorrect type
				printErr("Type error: attribute '%s' should have '%s' type\n", attr.name.c_str(), attr.type.c_str());
				return false;
			}
		}
	}
	printErr("No such attribute '%s'\n", attr_name.c_str());
	return false;
}

bool Table::insert(vector<string>& orders, vector<Value>& values) {
	map<string, Value> tuple;
	bool getPrimary = false;
	for (auto& attr : schema) {
		tuple[attr.name];	// construct all values as null value (in case orders.size() < schema.size())
	}
	for(int i=0; i<orders.size(); i++){
		// check data type
		if(not checkDataType(orders[i], values[i])){
			return false;
		}
		if(hasPrimary && orders[i] == primary_key_name){
			getPrimary = true;
			if(not checkPrimaryKey(values[i])){
				return false;
			}
			primary_key_columns.insert(values[i]);
		}
		tuple[orders[i]] = values[i];
	}
	if (hasPrimary and not getPrimary) {
		printErr("Value of primary key can't be null\n");
		return false;
	}
	if (not hasPrimary && std::find(tuples.begin(), tuples.end(), tuple) != tuples.end()) {
		// this find requires O(m), m == # of tuples
		// possible acceleration: use unordered_set<tuple> instead of vector<tuple>, tuple === map<string, Value>
		printErr("You can't insert exact the same tuple to a table\n");
		return false;
	}
	tuples.push_back(tuple);
	return true;
}

bool Table::insert(vector<Value>& values) {
	map<string, Value> tuple;
	for(int i=0; i<schema.size(); i++){
		if (i < values.size()) {
			// check data type
			if(not checkDataType(schema[i].name, values[i])){
				return false;
			}
			if(hasPrimary && schema[i].isPrimaryKey){
				if(not checkPrimaryKey(values[i])){
					return false;
				}
				primary_key_columns.insert(values[i]);
			}
			tuple[schema[i].name] = values[i];
		} else {
			// if the insert SQL give values.size() < schema.size(), the rest values are null
			if (schema[i].isPrimaryKey) {
				printErr("Value of primary key can't be null\n");
				return false;
			}
			// insert a null value by using empty constructor Value()
			tuple[schema[i].name];
		}
	}
	if (not hasPrimary && std::find(tuples.begin(), tuples.end(), tuple) != tuples.end()) {
		// this find requires O(m), m == # of tuples
		// possible acceleration: use unordered_set<tuple> instead of vector<tuple>, tuple === map<string, Value>
		printErr("You can't insert exact the same tuple to a table\n");
		return false;
	}
	tuples.push_back(tuple);
	return true;
}

bool BaseData::Query(string query_str){
	if (query_str == "show") {
		this->show();
		return true;
	}
	parser = new Parser(query_str);
	if(not parser->Parse()) return false;

	if(parser->isCreateTableQuery){
		// Create table
		Table table(parser->table_name, parser->schema);
		if(tables.find(table.table_name) != tables.end()){ // Duplicated table name
			printErr("Duplicated table name '%s'\n", table.table_name.c_str());
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
			printErr("No such table '%s'\n", parser->table_name.c_str());
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
const char* row_seperator(const vector<int>& column_widths) {
	static char buffer[2000];
	int pos = 0;
	for (int width : column_widths) {
		pos += sprintf(buffer + pos, "+%s", std::string(width, '-').c_str());
	}
	sprintf(buffer + pos, "+");
	return buffer;
}
void Table::show(){
	static const int OFFSET = 3;
	int i=0;

	// compute column widths to output
	vector<int> column_widths(schema.size());
	for (int i=0; i<schema.size(); i++) {
		column_widths[i] = schema[i].name.length();
	}
	for (auto& tuple : tuples){
		i=0;
		for (auto& attr : schema){
			column_widths[i] = std::max(column_widths[i], (int)tuple[attr.name].toString().length());
			i++;
		}
	}
	for (int& w:column_widths) {
		w += OFFSET;
	}

	

	printf("Table: %s\n", table_name.c_str());
	// Top '-' and '+'
	puts(row_seperator(column_widths));

	// output all atrribute name
	i=0;
	for (auto& attr : schema) {
		printf("|%*s", column_widths[i], attr.name.c_str());
		i++;
	}
	puts("|");
	puts(row_seperator(column_widths));

	if (tuples.size() == 0) return;
	// output all tuples
	for (auto& tuple : tuples){
		i=0;
		for (auto& attr : schema){
			printf("|%*s", column_widths[i], tuple[attr.name].toString().c_str());
			i++;
		}
		puts("|");
	}
	puts(row_seperator(column_widths));
}

string Table::schemaToString() {
	string ret;
	for (auto& attr : schema) {
		ret += attr.name;
		ret += ": ";
		ret += attr.type;
		if (attr.isPrimaryKey) ret += "(primary)";
		ret += ", ";
	}
	return ret;	
}
void BaseData::show() {
	std::cout << "Base: " << std::endl;
	for (auto& p: tables) {
		auto& table = p.second;
		std::cout << table.schemaToString();
		table.show();
	}
	puts("");
}
