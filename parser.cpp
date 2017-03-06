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
        isCreateTableQuery = true;
        if (Create_Table_Query() == false) return false;
    }
    else if(token == "insert") {
        isInsertQuery = true;
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
        fprintf(stderr, LIGHT_RED "Syntax Error: CREATE TABLE\n" WHITE);
        return false;
    }
    // Table name
    table_name = scanner.nextToken();
    // Check Schema
    token = scanner.nextToken();
    if(token != "("){
        fprintf(stderr, LIGHT_RED "Syntax Error: expected '('\n" WHITE);
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
        if(scanner.lookAhead() == ")"){
            scanner.nextToken();
            break;
        }
        else if(scanner.lookAhead() == ","){
            scanner.nextToken();
        }
        else{
            fprintf(stderr, LIGHT_RED "Syntax Error: expected ',' or ')'\n" WHITE);
            return false;
        }
    }
    return true;
}

bool Parser::Read_Attr_Def(Attribute& attr) {
    attr.name = scanner.nextToken();
    attr.type = scanner.nextToken();
    if(attr.type != "int" && attr.type != "varchar"){
        fprintf(stderr, LIGHT_RED "Error: unknown type '%s'\n" WHITE, attr.type.c_str());
        return false;
    }
    // Read char length
    if(attr.type == "varchar"){
        if(scanner.nextToken() != "("){
            fprintf(stderr, LIGHT_RED "Syntax Error: expected '('\n" WHITE);
            return false;
        }
        attr.char_len = stoi(scanner.nextToken());
        if(attr.char_len <= 0){
            fprintf(stderr, LIGHT_RED "char length should be greater than 0\n" WHITE);
            return false;
        }
        if(scanner.nextToken() != ")"){
            fprintf(stderr, LIGHT_RED "Syntax Error: expected ')'\n" WHITE);
            return false;
        }
    }
    // Check PRIMARY KEY
    if(scanner.lookAhead() == "primary"){
        scanner.nextToken();
        if(scanner.nextToken() != "key"){
            fprintf(stderr, LIGHT_RED "Syntax Error: do you mean 'PRIMARY KEY' ?\n" WHITE);
            return false;
        }
        attr.isPrimaryKey = true;
    }
    return true;
}


bool Parser::Insert_Query() {
    // "insert" // this is already consumed in Parse()
    
    // "into"
    string token = scanner.nextToken();
    if (token != "into") {
        fprintf(stderr, LIGHT_RED "Missing 'INTO' after 'INSERT'\n" WHITE);
        return false;
    }

    // table name
    table_name = scanner.nextToken();

    if (scanner.lookAhead() == "(") {
        // if there's diretly "(", call Read_Order_Specify()
        orderSpecified = true;
        if (not Read_Order_Specify()) return false;
    } 

    if(scanner.lookAhead() == "values") {
        // call Read_Value()
        scanner.nextToken();
        if (not Read_Value()) return false;
    } else {
        fprintf(stderr, LIGHT_RED "Expected VALUES', got %s\n" WHITE, scanner.lookAhead().c_str());
        return false;
    }

    // examine whether order.size() == values.size()
    if(orderSpecified && order.size() != values.size()){
        fprintf(stderr, LIGHT_RED "Specified counts must be same as value counts\n" WHITE);
        return false;
    }
    return true;
}

bool Parser::Read_Order_Specify() {
    string attr_name;
    order.clear();
    // "("
    scanner.nextToken();
    while(true) {
        // read attr_name1, attr_name2, attr_name3, ...
        attr_name = scanner.nextToken();
        // and order.push_back(attr_name)
        order.push_back(attr_name);
        // until ")"
        if(scanner.lookAhead() == ")"){
            scanner.nextToken();
            break;
        }
        else if(scanner.lookAhead() == ","){
            scanner.nextToken();
        }
        else{
            fprintf(stderr, LIGHT_RED "Syntax Error: expected ',' or ')'\n" WHITE);
            return false;
        }
    }
    return true;
}

bool Parser::Read_Value() {
    string value_name;
    values.clear();
    // "(" error detect
    if(scanner.lookAhead() != "("){
        fprintf(stderr, LIGHT_RED "Syntax Error: expected '(' after value\n" WHITE);
        return false;
    }
    scanner.nextToken();
    while(true) {
        // read value1, value2, value3, ...
        value_name = scanner.nextToken();
        // and values.push_back(value)
        values.push_back(value_name);
        // until ")"
        if(scanner.lookAhead() == ")"){
            scanner.nextToken();
            break;
        }
        else if(scanner.lookAhead() == ","){
            scanner.nextToken();
        }
        else{
            fprintf(stderr, LIGHT_RED "Syntax Error: expected ',' or ')'\n" WHITE);
            return false;
        }
    }
    return true;
}

void Parser::Print(){
    printf("SQL Statement:\n%s\n", query_str.c_str());
    printf("Table name: %s\n", table_name.c_str());
    if(isCreateTableQuery){
        for(auto &attr:schema) {
            printf("name: %s, type: %s, isPrimaryKey: %d, char_len: %d\n", attr.name.c_str(), attr.type.c_str(), attr.isPrimaryKey, attr.char_len);
        }
    }
    else if(isInsertQuery){
        if(orderSpecified){
            for(int i=0; i<order.size(); i++){
                printf("Insert %s: %s into TABLE(%s)\n", order[i].c_str(), values[i].c_str(), table_name.c_str());
            }
        }
        else{
            for(int i=0; i<values.size(); i++){
                printf("Insert %s into TABLE(%s)\n", values[i].c_str(), table_name.c_str());
            }
        }
    }
}
