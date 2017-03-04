#include "parser.hpp"
#include <cstdio>
#include <vector>

using namespace std;
Parser::Parser(string query_str): query_str(query_str), scanner(query_str) {}

void Parser::Parse(){
    string token;
    token = scanner.nextToken();
    if(token == "create")
        Create_Table_Query();
    else if(token == "insert")
        Insert_Query();
    else
        fprintf(stderr, "Syntax Error: unknown operator '%s'\n", token.c_str());
}

bool Parser::Create_Table_Query(){
    string token = scanner.nextToken(), table_name;
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

    Read_Schema();    
}

bool Parser::Read_Schema() {
    Attribute attr;
    while(true) {
        Read_Attr_Def(attr);
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
}


bool Parser::Insert_Query(){

}

void Parser::Print(){
    printf("SQL Statement: %s\n", query_str.c_str());
    printf("Table name: %s\n", table_name.c_str());
    for(auto &attr:schema) {
        printf("name: %s, type: %s, isPrimaryKey: %d, char_len: %d\n", attr.name.c_str(), attr.type.c_str(), attr.isPrimaryKey, attr.char_len);
    }
}
