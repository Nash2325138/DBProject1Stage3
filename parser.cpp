#include "parser.hpp"
#include "color.h"
#include <cstdio>
#include <vector>

using namespace std;
Parser::Parser(string query_str):   query_str(query_str), scanner(query_str), 
                                    isCreateTableQuery(false), isInsertQuery(false),
                                    orderSpecified(false) {}

bool Parser::Parse(){
    string token;
    token = scanner.nextToken();
    if(token == "create") {
        if (not Create_Table_Query()) return false;
    }
    else if(token == "insert") {
        if (not Insert_Query()) return false;
    }
    else {
        fprintf(stderr, LIGHT_RED "Syntax Error: unknown operator '%s'\n" WHITE, token.c_str());
        return false;
    }
    return true;
}

bool Parser::Create_Table_Query(){
    string token = scanner.nextToken();
    if(token != "table"){
        fprintf(stderr, "Syntax Error: CREATE TABLE\n");
        return false;
    }
    // Table name
    table_name = scanner.nextToken();
    // Check Schema
    token = scanner.nextToken();
    if(token != "("){
        fprintf(stderr, "Syntax Error: expected '('\n");
        return false;
    }
    schema.clear();

    if(not Read_Schema()) return false;   
    return true; 
}

bool Parser::Read_Schema() {
    Attribute attr;
    while(true) {
        if (not Read_Attr_Def(attr)) return false;
        schema.push_back(attr);
        if(scanner.lookAhead() == ")")
            break;
        if(scanner.lookAhead() == ",")
            scanner.nextToken();
        else{
            fprintf(stderr, "Syntax Error: expected ','\n");
            return false;
        }
    }
    return true;
}

bool Parser::Read_Attr_Def(Attribute& attr) {
    attr.name = scanner.nextToken();
    attr.type = scanner.nextToken();
    if(attr.type != "int" && attr.type != "varchar"){
        fprintf(stderr, "Error: unknown type '%s'\n", attr.type.c_str());
        return false;
    }
    // Read char length
    if(attr.type == "varchar"){
        if(scanner.nextToken() != "("){
            fprintf(stderr, "Syntax Error: expected '('\n");
            return false;
        }
        attr.char_len = stoi(scanner.nextToken());
        if(attr.char_len <= 0){
            fprintf(stderr, "char length should be greater than 0\n");
            return false;
        }
        if(scanner.nextToken() != ")"){
            fprintf(stderr, "Syntax Error: expected ')'\n");
            return false;
        }
    }
    // Check PRIMARY KEY
    if(scanner.lookAhead() == "primary"){
        scanner.nextToken();
        if(scanner.nextToken() != "key"){
            fprintf(stderr, "Syntax Error: do you mean 'PRIMARY KEY' ?\n");
            return false;
        }
        attr.isPrimaryKey = true;
    }
    return true;
}


bool Parser::Insert_Query() {
    // "insert"
    // "into"
    // table name
    // if there's diretly "(", call Read_Order_Specify()
    // call Read_Value()

    // examine whether order.size() == values.size()
    return true;
}
bool Parser::Read_Order_Specify() {
    // "("
    // read attr_name1, attr_name2, attr_name3, ...
    // and order.push_back(attr_name)
    // until ")"
    return true;
}
bool Parser::Read_Value() {
    // "values"
    // read value1, value2, value3, ...
    // and values.push_back(value)
    // until ")"
    return true;
}

void Parser::Print(){
    printf("SQL Statement: %s\n", query_str.c_str());
    printf("Table name: %s\n", table_name.c_str());
    for(auto &attr:schema) {
        printf("name: %s, type: %s, isPrimaryKey: %d, char_len: %d\n", attr.name.c_str(), attr.type.c_str(), attr.isPrimaryKey, attr.char_len);
    }
}
