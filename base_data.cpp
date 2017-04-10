#include <iostream>
#include <algorithm>
#include <regex>
#include <utility>
#include "base_data.hpp"
#include "color.h"

Table::Table(string& table_name, vector<Attribute>& schema): table_name(table_name), schema(schema)
															, hasPrimary(false){
	int i=0;
	for (auto& attr : schema) {
		if (attr.isPrimaryKey) {
			hasPrimary = true;
			primary_key_name = attr.name;
		}
		name_to_i[attr.name] = i++;
	}
}
Table::Table(){}
Table::~Table(){}

// our attribute chosen support regular expreesion 
const vector<int>& Table::matchedAttributes(const string& str) {
	static vector<int> matched;
	matched.clear();
	std::regex reg(str);
	for (int i=0; i<schema.size(); i++) {
		if (std::regex_match (schema[i].name, reg)) {
			matched.push_back(i);
		}
	}
	return matched;
}

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
	if (orders.size() > schema.size()) {
		printErr("Number of values exceeds number of atrributes\n");
		return false;
	}
	// construct all values as null value (in case orders.size() < schema.size())
	vector<Value> tuple(schema.size(), Value());
	// for (auto& attr : schema) {
	// 	tuple[attr.name];	// construct all values as null value (in case orders.size() < schema.size())
	// }

	bool getPrimary = false;
	Value primary_key_value;
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
			primary_key_value = values[i];
		}
		tuple[name_to_i[orders[i]]] = values[i];
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
	primary_key_columns.insert(primary_key_value);
	return true;
}

bool Table::insert(vector<Value>& values) {
	if(values.size() > schema.size()){
		printErr("Number of values exceeds number of atrributes \n");
		return false;
	}
	vector<Value> tuple(schema.size(), Value());
	Value primary_key_value;
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
				primary_key_value = values[i];
			}
			tuple[ name_to_i[schema[i].name] ]= values[i];
		} else {
			// if the insert SQL give values.size() < schema.size(), the rest values are null
			if (schema[i].isPrimaryKey) {
				printErr("Value of primary key can't be null\n");
				return false;
			}
			// insert a null value by using empty constructor Value()
			//***   we don't need the line below because declaration of tuple has 
			//      constructed it using empty constructor   ***// 
			// tuple[schema[i].name];
		}
	}
	if (not hasPrimary && std::find(tuples.begin(), tuples.end(), tuple) != tuples.end()) {
		// this find requires O(m), m == # of tuples
		// possible acceleration: use unordered_set<tuple> instead of vector<tuple>, tuple === map<string, Value>
		printErr("You can't insert exact the same tuple to a table\n");
		return false;
	}
	tuples.push_back(tuple);
	primary_key_columns.insert(primary_key_value);
	return true;
}

bool BaseData::select(Parser::SelectQueryData& sData) {
	// checkSelectQueryData(sData)
	if (not checkSelectQueryData(sData)) {
		return false;
	}

	// map from table to selected attributes;
	vector<pair<string, int>> selectedAttributes;

	// create an outputTable with schema concatenation of all selectedItems
	fillOutputTableSchema(sData, selectedAttributes);

	// for (tuple1 in table1):
	//   for (tuple2 in table2):
	//     if ( fullfill conditions of WHERE):
	//       1. if is aggregation function:
	//             do what aggregation want
	//       2. else:
	//             push_back one tuple with selectedItems to outputTable

	// outputTable.show();
	return true;
}
string BaseData::getTrueTableName(Parser::SelectQueryData& sData, string tableID) {
	auto it = sData.aliasToTableName.find(tableID);
	if ( it == sData.aliasToTableName.end()) {
		return tableID;
	} else {
		return it->second; 
	}
}
void BaseData::fillOutputTableSchema(Parser::SelectQueryData& sData,
						vector<pair<string, int>>& selectedAttributes) {
	for (auto& item: sData.selectedItems) {
		if (not item.isAggregation) {
			AttributeID attrID = item.attributeID;
			if (attrID.tableSpecified) {
				// get the table specified
				string trueName = getTrueTableName(sData, attrID.tableID);
				Table& specifiedTable = tables[trueName];

				// find those matched attribute name
				auto matched = specifiedTable.matchedAttributes(attrID.attr_name);
				for (auto& i: matched) {
					outputTable.schema.push_back(attrID.tableID+"."+specifiedTable.schema[i].name);
					selectedAttributes.push_back(make_pair(trueName, i));
				}
			} else {
				for (auto& fromTableStr: sData.fromTables) {
					Table& fromTable = tables[fromTableStr];
					auto matched = fromTable.matchedAttributes(attrID.attr_name);
					for (int i:matched) {
						outputTable.schema.push_back(fromTable.schema[i].name);
						selectedAttributes.push_back(make_pair(fromTableStr, i));
					}
				}
			}
		} else {
			outputTable.schema.push_back(item.toString());
		}
	}
}
bool BaseData::checkSelectQueryData(Parser::SelectQueryData& sData) {
	// 1. check if all fromTables in sData are also in Base

	// 2. check if all attributeID (in selected items or comparePairs) which is
	//		a. table specified: is in the specified table
	//		b. not table specified: exist in any fromTable and not ambiguous amoung fromTables
	// note: be careful of wildcard charactor: * (use matchedAttributes() to decide)

	// 3. all attributeID in comparePairs can't match two or more attributes 

	// 4. check if both side of compare pairs is the same type

	// 5. since we don't handle GROUP BY, if any aggregation function and normal attributes
	//    are in selected item together, it's a error

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
			tables[table.table_name] = table;	// std::move(table) ?
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
	else if(parser->isSelectQuery){
		
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
		// i=0;
		// for (auto& attr : schema){
		//	 column_widths[i] = std::max(column_widths[i], (int)tuple[name_to_i[attr.name]].toString().length());
		// 	i++;
		// }
		for (int i=0; i<schema.size(); i++) {
			column_widths[i] = std::max(column_widths[i], (int)tuple[i].toString().length());
		}
	}
	for (int& w:column_widths) {
		w += OFFSET;
	}

	printf("Table %s: %s\n", table_name.c_str(), this->schemaToString().c_str());
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
		// i=0;
		// for (auto& attr : schema){
		// 	printf("|%*s", column_widths[i], tuple[name_to_i[attr.name]].toString().c_str());
		// 	i++;
		// }
		for (int i = 0; i < schema.size(); ++i) {
			printf("|%*s", column_widths[i], tuple[i].toString().c_str());
		}
		puts("|");
	}
	puts(row_seperator(column_widths));
}
void OutputTable::show() {
	static const int OFFSET = 3;
	int i=0;

	// compute column widths to output
	vector<int> column_widths(schema.size());
	for (int i=0; i<schema.size(); i++) {
		column_widths[i] = schema[i].length();
	}
	for (auto& tuple : tuples){
		for (int i=0; i<schema.size(); i++) {
			column_widths[i] = std::max(column_widths[i], (int)tuple[i].toString().length());
		}
	}
	for (int& w:column_widths) {
		w += OFFSET;
	}

	// Top '-' and '+'
	puts(row_seperator(column_widths));

	// output all atrribute name
	i=0;
	for (auto& attr : schema) {
		printf("|%*s", column_widths[i], attr.c_str());
		i++;
	}
	puts("|");
	puts(row_seperator(column_widths));

	if (tuples.size() == 0) return;
	// output all tuples
	for (auto& tuple : tuples){
		for (int i = 0; i < schema.size(); ++i) {
			printf("|%*s", column_widths[i], tuple[i].toString().c_str());
		}
		puts("|");
	}
	puts(row_seperator(column_widths));
}
string Table::schemaToString() {
	char ret[2000];
	int pos = 0;
	pos += sprintf(ret+pos, NONE "(" YELLOW);
	for (int i=0; i<schema.size(); i++) {
		if (i) pos += sprintf(ret+pos, NONE ", " YELLOW);
		auto& attr = schema[i];
		pos += sprintf(ret+pos, "%s %s", attr.name.c_str(), attr.type.c_str());
		if (attr.isPrimaryKey) pos += sprintf(ret+pos, CYAN " primary" YELLOW);
	}
	pos += sprintf(ret+pos, NONE ")");
	return ret;	
}
void BaseData::show() {
	for (auto& p: tables) {
		auto& table = p.second;
		table.show();
	}
	puts("");
}
