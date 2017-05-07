#include <iostream>
#include <algorithm>
#include <regex>
#include <utility>
#include <string>
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
const vector<int>& Table::matchedAttributes(string str) {
	size_t index = 0;
	while (true) {
	     /* Locate the '*' to replace. */
	     index = str.find('*', index);
	     if (index == std::string::npos) break;

	     /* Make the replacement. */
	     str.replace(index, 1, ".*");

	     /* Advance index forward so the next iteration doesn't pick it up as well. */
	     index += 2;
	}
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

bool Table::writeNewRecord(const vector<Value> tuple, const string &table_name, const int &row){
	char output_path[] = "datas/";
	strcat(output_path, (table_name + "_").c_str());
	strcat(output_path, row + '0');
	ofstream ofs(output_path);
	boost::archive::text_iarchive oa(ofs);
	oa << tuple;
	return true;
}

bool BaseData::judgeComparePair(Value* v1, CompareOP op, Value* v2) {
	if (op == CompareOP::OP_EMPTY) {
		return v1->isTrue();
	} else if (op == CompareOP::BIGGER) {
		return v1->bigger(*v2);
	} else if (op == CompareOP::SMALLER) {
		return v1->smaller(*v2);
	} else if (op == CompareOP::EQUAL) {
		return v1->equal(*v2);
	} else if (op == CompareOP::NOT_EQUAL) {
		return !v1->equal(*v2);
	} else {
		return false;
	}
}
bool BaseData::judgeWhere(Parser::SelectQueryData& sData, pair<Table*, int>& t1_row) {
	if (sData.comparePairs.size() == 0) {
		return true;
	}
	bool ans = false;
	for (int i = 0; i < sData.comparePairs.size(); ++i) {
		CompareOP op = sData.comparePairs[i].op;
		Value *v1, *v2;
		Table_col& table_col1 = comparePairs_table_col[i].first;
		Table_col& table_col2 = comparePairs_table_col[i].second;

		if (table_col1.first != NULL) {
			v1 = &table_col1.first->tuples[t1_row.second][table_col1.second];
		} else {
			v1 = &sData.comparePairs[i].v1;
		}
		if (table_col2.first != NULL) {
			v2 = &table_col2.first->tuples[t1_row.second][table_col2.second];
		} else {
			v2 = &sData.comparePairs[i].v2;
		}
		bool pairResult = judgeComparePair(v1, op ,v2);
		if (i == 0) {
			ans |= pairResult;
		} else {
			if (sData.logicalOP == LogicalOP::AND) {
				ans &= pairResult;
			} else if (sData.logicalOP == LogicalOP::OR) {
				ans |= pairResult;
			} else {
				fprintf(stderr, "No such LogicalOP: %d\n", sData.logicalOP);
				exit(EXIT_FAILURE);
			}
		}
	}
	return ans;
}
bool BaseData::judgeWhere(Parser::SelectQueryData& sData, pair<Table*, int>& t1_row, pair<Table*, int>& t2_row) {
	if (sData.comparePairs.size() == 0) {
		return true;
	}
	bool ans = false;
	for (int i = 0; i < sData.comparePairs.size(); ++i) {
		CompareOP op = sData.comparePairs[i].op;
		Value *v1, *v2;
		Table_col& table_col1 = comparePairs_table_col[i].first;
		Table_col& table_col2 = comparePairs_table_col[i].second;

		if (table_col1.first != NULL) {
			auto& table_row = (t1_row.first == table_col1.first) ? t1_row:t2_row;
			v1 = &table_col1.first->tuples[table_row.second][table_col1.second];
		} else {
			v1 = &sData.comparePairs[i].v1;
		}
		if (table_col2.first != NULL) {
			auto& table_row = (t1_row.first == table_col2.first) ? t1_row:t2_row;
			v2 = &table_col2.first->tuples[table_row.second][table_col2.second];
		} else {
			v2 = &sData.comparePairs[i].v2;
		}
		// printf("Comparing v1: %s with v2: %s, op = %d\n", v1->toString().c_str(), v2->toString().c_str(), op);
		bool pairResult = judgeComparePair(v1, op ,v2);
		// printf("result == %d\n", pairResult);
		// printf("sData.comparePairs.size(): %d\n", sData.comparePairs.size());
		if (i == 0) {
			ans |= pairResult;
		} else {
			if (sData.logicalOP == LogicalOP::AND) {
				ans &= pairResult;
			} else if (sData.logicalOP == LogicalOP::OR) {
				ans |= pairResult;
			} else {
				fprintf(stderr, "No such LogicalOP: %d\n", sData.logicalOP);
				exit(EXIT_FAILURE);
			}
		}
	}
	return ans;
}
void BaseData::push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row) {
	Tuple tuple;
	for (auto& p: selectedAttributes) {
		if (p.first != t1_row.first) {
			printErr("selectedAttribute not in from tables!!!??");
			exit(EXIT_FAILURE);
		}
		tuple.push_back(t1_row.first->tuples.at(t1_row.second).at(p.second));
	}
	outputTable.tuples.push_back(tuple);
}
void BaseData::push_back_output(vector<pair<Table*, int> >& selectedAttributes, pair<Table*, int> t1_row, pair<Table*, int> t2_row) {
	Tuple tuple;
	for (auto& p: selectedAttributes) {
		if (p.first == t1_row.first) {
			tuple.push_back(t1_row.first->tuples.at(t1_row.second).at(p.second));
		} else if (p.first == t2_row.first) {
			tuple.push_back(t2_row.first->tuples.at(t2_row.second).at(p.second));
		} else {
			printErr("selectedAttribute not in from tables!!!??");
			exit(EXIT_FAILURE);
		}
	}
	outputTable.tuples.push_back(tuple);
}
Table* BaseData::getSourceTable(AttributeID& attrID) {
	// if table specified
	if(attrID.tableSpecified){
		return &(tables[getTrueTableName(*(parser->selectData), attrID.tableID)]);
	}
	//
	else{
		for(auto &table_name : parser->selectData->fromTables){
			Table &table = tables[table_name];
			if(table.matchedAttributes(attrID.attr_name).size() > 0){
				return &table;
			}
		}
	}
	return NULL;
}
bool BaseData::select(Parser::SelectQueryData& sData) {
	outputTable.clear();
	// checkSelectQueryData(sData)
	if (not checkSelectQueryData(sData)) {
		return false;
	}

	// map from table to selected attributes
	vector<Table_col> selectedAttributes;
	// map from comparePair to table_col 
	comparePairs_table_col = vector<pair<Table_col, Table_col> >(sData.comparePairs.size());
	for (int i=0; i<sData.comparePairs.size(); i++)
	{
		auto& comparePair = sData.comparePairs[i];
		if (comparePair.type1 == CompareType::ATTRIBUTE) {
			Table_col& table_col = comparePairs_table_col[i].first;
			table_col.first = getSourceTable(comparePair.attrID1);
			if (table_col.first != NULL) {
				table_col.second = table_col.first->matchedAttributes(comparePair.attrID1.attr_name)[0];
			}
		} else {
			comparePairs_table_col[i].first.first = NULL;
		}
		if (comparePair.type2 == CompareType::ATTRIBUTE) {
			Table_col& table_col = comparePairs_table_col[i].second;
			table_col.first = getSourceTable(comparePair.attrID2);
			if (table_col.first != NULL) {
				table_col.second = table_col.first->matchedAttributes(comparePair.attrID2.attr_name)[0];
			}
		} else {
			comparePairs_table_col[i].second.first = NULL;
		}
	}

	// create an outputTable with schema concatenation of all selectedItems
	fillOutputTableSchema(sData, selectedAttributes);

	bool isAggregation = (selectedAttributes.size() == 0);
	if (isAggregation) {
		// Tulpe tuple(sData.selectedItems.size(), Value("0"));
		outputTable.tuples.emplace_back(sData.selectedItems.size(), Value("0"));
	}
	// one table
	if (sData.fromTables.size() == 1) {
		Table& t = tables[sData.fromTables[0]]; // fromTable must be true name
		for (int i=0 ; i<t.tuples.size(); i++) {
			pair<Table*, int> table_row = make_pair(&t, i);
			if (judgeWhere(sData, table_row) == false) continue;
			if (not isAggregation) {
				push_back_output(selectedAttributes, table_row);
			} else {
				// is aggregation
				for (int j=0; j<sData.selectedItems.size(); j++) {
					auto& item = sData.selectedItems[j];
					if (item.aggreFuncStr == "sum") {
						Table& table = *getSourceTable(item.attributeID);
						auto& indexes = table.matchedAttributes(item.attributeID.attr_name);
						/*** these checks can be put outside for loop  ***/
						if (indexes.size() > 1) {
							printErr("We can't sum up multiple attributes\n");
							return false;
						}
						if ( table.schema[indexes[0]].type != "int") {
							printErr("Type error: type %s can't be aggregated\n", table.schema[indexes[0]].type.c_str());
							return false;
						}
						outputTable.tuples[0][j].intData += table.tuples[i][indexes[0]].intData;
					} else if (item.aggreFuncStr == "count") {
						Table& table = *getSourceTable(item.attributeID);
						auto& indexes = table.matchedAttributes(item.attributeID.attr_name);
						for (int index: indexes) {
							if (not table.tuples[i][index].isNull) {
								outputTable.tuples[0][j].intData++;
								break;
							}
						}
					}
				} 
			}
		}
	}

	// two tables
	else if (sData.fromTables.size() == 2) {
		Table& t1 = tables[sData.fromTables[0]];
		Table& t2 = tables[sData.fromTables[1]];
		for (int i=0; i<t1.tuples.size(); ++i)
		{
			pair<Table*, int> t1_row = make_pair(&t1, i);
			for (int j=0; j<t2.tuples.size(); ++j)
			{
				pair<Table*, int> t2_row = make_pair(&t2, j);
				if (judgeWhere(sData, t1_row, t2_row) == false) continue;
				if (not isAggregation) {
					push_back_output(selectedAttributes, t1_row, t2_row);
				} else {
					// is aggregation
					for (int k=0; k<sData.selectedItems.size(); k++) {
						auto& item = sData.selectedItems[k];
						if (item.aggreFuncStr == "sum") {
							Table& table = *getSourceTable(item.attributeID);
							auto& indexes = table.matchedAttributes(item.attributeID.attr_name);
							/*** these checks can be put outside for loop  ***/
							if (indexes.size() > 1) {
								printErr("We can't sum up multiple attributes\n");
								return false;
							}
							if ( table.schema[indexes[0]].type != "int") {
								printErr("Type error: type %s can't be aggregated\n", table.schema[indexes[0]].type.c_str());
								return false;
							}
							outputTable.tuples[0][k].intData += table.tuples[i][indexes[0]].intData;
						} else if (item.aggreFuncStr == "count") {
							Table& table = *getSourceTable(item.attributeID);
							auto& indexes = table.matchedAttributes(item.attributeID.attr_name);
							for (int index: indexes) {
								if (not table.tuples[i][index].isNull) {
									outputTable.tuples[0][k].intData++;
									break;
								}
							}
						}
					} 
				}
			}
		}
	}
	// for (tuple1 in table1):
	//   for (tuple2 in table2):
	//     if ( fullfill conditions of WHERE):
	//       1. if is aggregation function:
	//             do what aggregation want
	//       2. else:
	//             push_back one tuple with selectedItems to outputTable

	outputTable.show();
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
						vector<pair<Table*, int> >& selectedAttributes) {
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
					selectedAttributes.push_back(make_pair(&specifiedTable, i));
				}
			} else {
				for (auto& fromTableStr: sData.fromTables) {
					Table& fromTable = tables[fromTableStr];
					auto matched = fromTable.matchedAttributes(attrID.attr_name);
					for (int i:matched) {
						outputTable.schema.push_back(fromTable.schema[i].name);
						selectedAttributes.push_back(make_pair(&fromTable, i));
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
	if(not checkTableExistence(sData.fromTables)){
		return false;
	}
	
	// 2. check if all attributeID (in selected items or comparePairs) which is
	//		a. table specified: is in the specified table
	//		b. not table specified: exist in any fromTable and not ambiguous amoung fromTables
	// note: be careful of wildcard charactor: * (use matchedAttributes() to decide)
	if(not checkAttributeStatus(sData)){
		return false;
	}

	// 3. check if both side of compare pairs is the same type
	if(not checkPairTypes(sData)){
		return false;
	}
	
	// 4. since we don't handle GROUP BY, if any aggregation function and normal attributes
	//    are in selected item together, it's a error
	bool has_aggregation = false;
	int aggregation_count = 0;
	for(auto &item : sData.selectedItems){
		if(item.isAggregation){
			has_aggregation = true;
			aggregation_count++;
		}
	}
	if(has_aggregation && sData.selectedItems.size() > aggregation_count){
		printErr("The attributes without aggregation function should be grouped by\n");
		return false;
	}

	return true;
}

bool BaseData::checkTableExistence(vector<string> &fromTables){
	for(auto &name : fromTables){
		if(tables.find(name) == tables.end()){
			printErr("No such table '%s'\n", name.c_str());
			return false;
		}
	}
	return true;
}

bool BaseData::checkAttributeStatus(Parser::SelectQueryData &selectedData){
	vector<AttributeID> attrIDs;
	attrIDs.clear();
	// get select items & comparepairs
	for(auto &item : selectedData.selectedItems){
		attrIDs.push_back(item.attributeID);
	}
	for(auto &pair : selectedData.comparePairs){
		if(pair.type1 == CompareType::ATTRIBUTE){
			attrIDs.push_back(pair.attrID1);
		}
		if(pair.type2 == CompareType::ATTRIBUTE){
			attrIDs.push_back(pair.attrID2);
		}
	}

	for(auto &attrID : attrIDs){
		// table specified
		if(attrID.tableSpecified){
			string table_name = getTrueTableName(selectedData, attrID.tableID);
			if(tables[table_name].matchedAttributes(attrID.attr_name).size() == 0){
				printErr("No such attribute '%s' in table '%s'\n", attrID.attr_name.c_str(), table_name.c_str());
				return false;
			}
		}
		else{	// not table specified
			int numOfThisAttr = 0;
			for(auto &table_name : selectedData.fromTables){
				Table &table = tables[table_name];
				numOfThisAttr += table.matchedAttributes(attrID.attr_name).size();
			}
			if(numOfThisAttr == 0){
				printErr("No such attribute '%s'\n", attrID.attr_name.c_str());
				return false;
			}
			else if(numOfThisAttr > 1 and attrID.attr_name.find('*') == string::npos){
				printErr("Ambiguous attribute name '%s'\n", attrID.attr_name.c_str());
				return false;
			}
		}
	}
	return true;
}

bool BaseData::checkPairTypes(Parser::SelectQueryData& selectedData){
	for(auto &pair : selectedData.comparePairs){
		string type1, type2;
		if(pair.op == CompareOP::OP_EMPTY){ continue; }
		// Attr & Attr
		if(pair.type1 == CompareType::ATTRIBUTE && pair.type2 == CompareType::ATTRIBUTE){
			type1 = getAttributeType(pair.attrID1);
			type2 = getAttributeType(pair.attrID2);
		}
		// Attr & !Attr
		else if(pair.type1 == CompareType::ATTRIBUTE && pair.type2 != CompareType::ATTRIBUTE){
			type1 = getAttributeType(pair.attrID1);
			type2 = getTypeString(pair.type2);
		}
		// !Attr & Attr
		else if(pair.type1 != CompareType::ATTRIBUTE && pair.type2 == CompareType::ATTRIBUTE){
			type1 = getTypeString(pair.type1);
			type2 = getAttributeType(pair.attrID2);
		}
		// !Attr & !Attr
		else {
			type1 = getTypeString(pair.type1);
			type2 = getTypeString(pair.type2);
		}
		if(type1 != type2){
			printErr("Cannot compare '%s' with '%s'\n", type1.c_str(), type2.c_str());
			return false;
		}
	}
	return true;
}

string BaseData::getTypeString(CompareType CT){
	if(CT == CompareType::INT_CONST){ return "int"; }
	return "varchar";
}

string BaseData::getAttributeType(AttributeID attrID){
	if(attrID.tableSpecified){
		Table &table = tables[getTrueTableName(*(parser->selectData), attrID.tableID)];
		vector<int> pos = table.matchedAttributes(attrID.attr_name);
		return table.schema[pos[0]].type;
	}
	else{
		for(auto &table_name : parser->selectData->fromTables){
			Table &table = tables[table_name];
			vector<int> pos;
			if((pos = table.matchedAttributes(attrID.attr_name)).size() > 0){
				return table.schema[pos[0]].type;
			}
		}
	}
	return "";
}

bool BaseData::Query(string query_str){
	if (regex_match(query_str, regex("[ \n\t]*show[ \n\t]*"))) {
		this->show();
		return true;
	} else if (regex_match(query_str, regex("[ \n\t]*load[ \n\t]*[^ \n\t]*[ \n\t]*"))) {
		char fileName[100];
		char buffer[1000];
		sscanf(query_str.c_str()," %*s %s", fileName);
		FILE * fp = fopen(fileName, "r");
		if (fp == NULL) {
			perror("Error when reading file");
			return false;
		}
		printf(LIGHT_GREEN "\nLoading file: %s... \n" NONE, fileName);
		while( fscanf(fp, " %[^;];", buffer) > 0 ) {
			this->Query(string(buffer));
		}
		printf("Done\n");
		return true;
	} else if (regex_match(query_str, regex("[ ]*quit[ \n\t]*"))) {
		// Saving the data into 'base.save'
		printf("Saving the data into 'base.save' ...\n");
		{
			std::ofstream ofs("base.save");
			boost::archive::text_oarchive oa(ofs);
			oa << *this;
		}
		exit(EXIT_SUCCESS);
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
		if (not select(*parser->selectData)) return false;
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
