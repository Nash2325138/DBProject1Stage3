#include "parser.hpp"
#include "color.h"
#include <cstdio>
#include <vector>
#include <map>

using namespace std;
Parser::Parser(string query_str):   query_str(query_str), scanner(query_str), 
                                    isCreateTableQuery(false), isInsertQuery(false),
                                    orderSpecified(false) {}

bool Parser::validName(string& name) {
    static const std::regex reg("[a-zA-Z_][a-zA-Z_0-9]*");
    return std::regex_match (name, reg); 
}
bool Parser::isIntString(const string& str) {
    static const std::regex reg("-?[0-9]+");
    return std::regex_match (str, reg); 
}
bool Parser::isStrString(const string& str) {
    static const std::regex reg1("\"[^\"]*\"");
    static const std::regex reg2("\'[^\']*\'");
    return std::regex_match (str, reg1) || std::regex_match(str, reg2); 
}
bool Parser::Parse(){
    string token;
    token = scanner.nextToken();
    if(token == "create") {
        isCreateTableQuery = true;
        if (not Create_Table_Query()) return false;
    }
    else if(token == "insert") {
        isInsertQuery = true;
        if (not Insert_Query()) return false;
    }
    else {
        // setbuf(stdout, NULL);
        // printf("???\n");
        printErr("Syntax Error: unknown operator '%s'\n", token.c_str());
        // printf("???\n");
        return false;
    }
    int num_chars = 0;
    sscanf(scanner.lookAhead().c_str(), " %n", &num_chars);
    if (num_chars > 0) {
        printErr("Unexpected token '%s' after a complete SQL query\n", scanner.lookAhead().c_str());
        return false;
    }
    return true;
}

bool Parser::Create_Table_Query(){
    string token = scanner.nextToken();
    if(token != "table"){
        printErr("Syntax Error: CREATE TABLE\n");
        return false;
    }
    // Table name
    table_name = scanner.nextToken();
    if (not validName(table_name)) {
        printErr("Table name invalid\n");
        return false;
    }
    // Check Schema
    token = scanner.nextToken();
    if(token != "("){
        printErr( "Syntax Error: expected '('\n");
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
            printErr("Syntax Error: expected ',' or ')'\n");
            return false;
        }
    }

    // check duplicated attribute name
    map<string, int> attr_name_times;
    for (auto& attr : schema) {
        attr_name_times[attr.name]++;
    }
    for (auto& p : attr_name_times) {
        if (p.second > 1) {
            printErr("Error: Duplicated attribute name for \'%s\'\n", p.first.c_str());
            return false;
        }
    }

    return true;
}

bool Parser::Read_Attr_Def(Attribute& attr) {
    attr.name = scanner.nextToken();
    if (not validName(attr.name)) {
        printErr("Attribute name invalid\n");
        return false;
    }
    attr.type = scanner.nextToken();
    attr.isPrimaryKey = false;
    if(attr.type != "int" && attr.type != "varchar"){
        printErr("Error: unknown type '%s'\n", attr.type.c_str());
        return false;
    }
    // Read char length
    if(attr.type == "varchar"){
        if(scanner.nextToken() != "("){
            printErr("Syntax Error: expected '('\n");
            return false;
        }
        attr.char_len = stoi(scanner.nextToken());
        if(attr.char_len <= 0){
            printErr("char length should be greater than 0\n");
            return false;
        }
        if(scanner.nextToken() != ")"){
            printErr("Syntax Error: expected ')'\n");
            return false;
        }
    }
    // Check PRIMARY KEY
    if(scanner.lookAhead() == "primary"){
        scanner.nextToken();
        if(scanner.nextToken() != "key"){
            printErr("Syntax Error: do you mean 'PRIMARY KEY' ?\n");
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
        printErr("Missing 'INTO' after 'INSERT'\n");
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
        printErr("Expected 'VALUES', got %s\n", scanner.lookAhead().c_str());
        return false;
    }

    // examine whether orders.size() == values.size()
    if(orderSpecified && orders.size() != values.size()){
        printErr("Specified counts must be same as value counts\n");
        return false;
    }
    return true;
}

bool Parser::Read_Order_Specify() {
    string attr_name;
    orders.clear();
    // "("
    scanner.nextToken();
    while(true) {
        // read attr_name1, attr_name2, attr_name3, ...
        attr_name = scanner.nextToken();
        // and orders.push_back(attr_name)
        orders.push_back(attr_name);
        // until ")"
        if(scanner.lookAhead() == ")"){
            scanner.nextToken();
            break;
        }
        else if(scanner.lookAhead() == ","){
            scanner.nextToken();
        }
        else{
            printErr("Syntax Error: expected ',' or ')'\n");
            return false;
        }
    }
    return true;
}

bool Parser::Read_Value() {
    Value value;
    values.clear();
    // "(" error detect
    if(scanner.lookAhead() != "("){
        printErr("Syntax Error: expected '(' after value\n");
        return false;
    }
    scanner.nextToken();
    while(true) {
        // read value1, value2, value3, ...
        if (scanner.lookAhead() == "," || scanner.lookAhead() == ")") {
            value = Value();
        } else {
            try {
                value = Value(scanner.lookAhead());
            } catch (const std::invalid_argument& ia) {
                printErr("Type Error: expected number or string, got '%s'\n", scanner.lookAhead().c_str());
                return false;
            };
            scanner.nextToken();
        }

        // and values.push_back(value)
        values.push_back(value);
        // until ")"
        if(scanner.lookAhead() == ")"){
            scanner.nextToken();
            break;
        }
        else if(scanner.lookAhead() == ","){
            scanner.nextToken();
        }
        else{
            printErr("Syntax Error: expected ',' or ')'\n");
            return false;
        }
    }
    return true;
}

bool Parser::Select_Query() {
    // read_Selected_Item_Sequence() to fill:
    //      selectData->selectedItems
    selectData = new Parser::SelectQueryData();
    if(not read_Selected_Item_Sequence()) 
        return false;

    // read_FromTable_Sequence() to fill
    //      1. selectData->fromTables
    //      2. selectData->aliasToTableName
    
    // if lookagead == "where":
    // read_Where_Clause() to fill 
    //      1. selectData->comparePairs 
    //      2. selectData->logicalOP
    
    return true;
}

bool Parser::read_Selected_Item_Sequence() {
    // while (1) { read_Selected_Item() and read "," until lookahead == "" or "from" }
    string dot;
    while(read_Selected_Item()){
        if(scanner.lookAhead() == "from")
            return true;
        else if(scanner.lookAhead() == ""){
            printErr("Syntax Error: expected 'FROM'\n");
            return false;
        }
        else if(scanner.lookAhead() != ","){
            printErr("Syntax Error: expected ','\n");
            return false;
        }
        dot = scanner.nextToken();
    }
    return false;
}

bool Parser::read_Selected_Item() {
    // A selected item can be either AggrFunc(attrID) or attrID
    //      1. AggrFunc can either be "SUM" or "COUNT"
    //      2. use read_AttrID(attrID_ref) to read and fill a attrID
    if(scanner.lookAhead() == "" || scanner.lookAhead() == "from"){
        printErr("Syntax Error: expected attribute names or aggregate function\n");
        return false;
    }

    if(scanner.lookAhead() == "sum" || scanner.lookAhead() == "count"){
        string aggreName = scanner.nextToken();
        
        if(scanner.lookAhead() != "("){
            printErr("Syntax Error: expected '(' after %s\n", aggreName.c_str());
            return false;
        }
        scanner.nextToken(); // read '('
        
        AttributeID id;
        if(not read_AttrID(id))
            return false;
        
        if(scanner.lookAhead() != ")"){
            printErr("Syntax Error: expected ')' after %s\n", aggreName.c_str());
            return false;
        }
        scanner.nextToken(); // read ')'

        // Parser::SelectedItem item(aggreName, id);
        selectData->selectedItems.emplace_back(aggreName, id);
    }
    else{
        AttributeID id;
        if(not read_AttrID(id)) 
            return false;
        // Parser::SelectedItem item(id);
        selectData->selectedItems.emplace_back(id);
    }
    return false;
}

bool Parser::read_AttrID(AttributeID& attrID) {
    // An attrID can be either an attrName or tableID.attrName
    //      1. An attrName is a string
    //      2. An tableID can be either alias or table name, but they are all string
    string attrName = scanner.nextToken();
    if(scanner.lookAhead() == "."){
        scanner.nextToken();
        attrID = AttributeID(attrName, scanner.nextToken());
    }
    else{
        attrID = AttributeID(attrName);
    }
    return true;
}

bool Parser::read_FromTable_Sequence() {
    // read "from"
    if (scanner.nextToken() != "from") {
        printErr("Syntax error: expected 'FROM'\n");
        return false;
    }
    // while (1) {read_FromTable() and read "," until lookagead == "" or "where"}
    while (read_FromTable()){
        const string& ahead = scanner.lookAhead();
        if (ahead == "") { // if there can be other string behind, append condition (or ahead == "something")
            return true;
        }
        else if (ahead == ","){
            scanner.nextToken();
            continue;
        } else {
            printErr("Syntax error: unexpected '%s'", ahead.c_str());
            return false;
        }
    }
    return true;
}

bool Parser::read_FromTable() {
    // read "tableName" or "tableName as alias" 
    //      1. push_back or emplace_back to selectData->fromTables
    //      2. insert (alias, tableName) to selectData->aliasToTableName
    string tableName = scanner.nextToken();
    if (not Parser::validName(tableName)) {
        printErr("Syntax error: unexpected '%s' (won't be valid name of table)\n", tableName.c_str());
        return false;
    }
    selectData->fromTables.push_back(tableName);
    if (scanner.lookAhead() == "as") {
        scanner.nextToken();
        string alias = scanner.nextToken();
        if (not Parser::validName(alias)) {
            printErr("Syntax error: invalid alias '%s'\n", alias.c_str());
            return false;
        }
        if (selectData->aliasToTableName.find(alias) != selectData->aliasToTableName.end()) {
            printErr("Alias '%s' has been used\n", alias.c_str());
            return false;
        } else {
            selectData->aliasToTableName[alias] = tableName;
            return true;
        }
    }
    return true;
}

bool Parser::read_Where_Clause() {
    // 1. read "where"
    // 2. read_ComparePair()
    // 3. if (lookahead == "or" or "and"):
    //      read_ComparePair()
    scanner.nextToken(); // read "where"

    Parser::ComparePair pair;
    if(not read_ComparePair(pair))
        return false;
    comparePairs.push_back(pair);

    if(scanner.lookAhead() == "or" || scanner.lookAhead() == "and"){
        if(not read_ComparePair(pair))
            return false;
        comparePairs.push_back(pair);
    }
    // we don't need to handle more than two conditions in the WHERE clause 
    return true;
}

bool Parser::read_ComparePair(ComparePair& cmpPair) {
    bool isFrontAttr = false, isBackAttr = false;
    AttributeID attr1, attr2;
    string op;

    read_AttrID(attr1);

    op = scanner.nextToken();
    if(op == "<" && scanner.lookAhead() == ">")
        op += scanner.nextToken();

    read_AttrID(attr2);

    if(isStrString(attr1.attr_name) || isIntString(attr1.attr_name))
        isFrontAttr = false;
    if(isStrString(attr2.attr_name) || isIntString(attr2.attr_name))
        isBackAttr = false;

    if(isFrontAttr && isBackAttr)
        cmpPair = ComparePair(attr1, op, attr2);
    else if(isFrontAttr && !isBackAttr)
        cmpPair = ComparePair(attr1, op, attr2.attr_name);
    else if(!isFrontAttr && isBackAttr)
        cmpPair = ComparePair(attr1.attr_name, op, attr2);
    else if(!isFrontAttr && !isBackAttr)
        cmpPair = ComparePair(attr1.attr_name, op, attr2.attr_name);

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
            for(int i=0; i<orders.size(); i++){
                printf("Insert %s: %s into TABLE(%s)\n", orders[i].c_str(), values[i].toString().c_str(), table_name.c_str());
            }
        }
        else{
            for(int i=0; i<values.size(); i++){
                printf("Insert %s into TABLE(%s)\n", values[i].toString().c_str(), table_name.c_str());
            }
        }
    }
}
