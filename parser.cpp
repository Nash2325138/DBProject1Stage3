#include <cstdio>
#include <vector>
#include "scanner.hpp"
#include "parser.hpp"

using namespace std;

vector<Attribute> schema;

void Parser::Parse(Scanner scanner){
    string token;
    token = scanner.nextToken();
    if(token == "create")
        Create_Table_Query(scanner);
    else if(token == "insert")
        Insert_Query(scanner);
    else
        printf("Syntax Error: unknown operator '%s'\n", token);
}

bool Parser::Create_Table_Query(Scanner scanner){
    string token = scanner.nextToken(), table_name;
    if(token != "table"){
        printf("Syntax Error: CREATE TABLE\n");
        return false;
    }
    // Table name
    table_name = scanner.nextToken();
    // Schema
    token = scanner.nextToken();
    if(token != "("){
        printf("Syntax Error: expected '('\n");
        return false;
    }
    schema.clear();
    Attribute attr = new Attribute();
    while(scanner.lookAhead() != ")"){
        attr.name = scanner.nextToken();
        attr.type = scanner.nextToken();
        if(attr.type != "int" || attr.type != "varchar"){
            printf("Error: unknown type '%s'\n", attr.type);
            return false;
        }
        
    }
}

bool Parser::Insert_Query(Scanner scanner){

}

