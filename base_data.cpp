#include "base_data.hpp"

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