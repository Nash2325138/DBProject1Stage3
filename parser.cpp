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
    // Schema
    token = scanner.nextToken();
    if(token != "("){
        fprintf(stderr, "Syntax Error: expected '('\n");
        return false;
    }
    schema.clear();
    // Attribute attr = new Attribute();
    // while(scanner.lookAhead() != ")"){
    //     attr.name = scanner.nextToken();
    //     attr.type = scanner.nextToken();
    //     if(attr.type != "int" || attr.type != "varchar"){
    //         fprintf(stderr, "Error: unknown type '%s'\n", attr.type);
    //         return false;
    //     }    
    // }
    Read_Schema();
    
}

bool Parser::Read_Schema() {
    while(1) {
        Attribute attr;
        Read_Attr_Def(attr);
        schema.push_back(attr);
    }
}
bool Parser::Read_Attr_Def(Attribute& attr) {

}


bool Parser::Insert_Query(){

}



