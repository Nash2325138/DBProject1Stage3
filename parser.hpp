#ifndef PARSER_HPP
#define PARSER_HPP

/* Parser.hpp */
#include "scanner.hpp"
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class Attribute{
public:
    string name;
    string type;
    bool isPrimaryKey;
    int char_len;

    Attribute(){
        name = "";
        type = "";
        isPrimaryKey = false;
        char_len = 0;
    }
};

class Value
{
public:
    bool isNull;
    bool isInt;
    bool isString;
    
    int intData;
    string strData;

    Value() {
        isNull = true;
    }
    Value (const string& str) {
        isNull = false;
        if ( (str.front() == '\"' && str.back()=='\"') ||
             (str.front() == '\'' && str.back()=='\'')    ) {
            isString = true;
            isInt = false;
            strData = string(str.begin()+1, str.end()-1);
        } else {
            // may throw const std::invalid_argument
            // catch it outside
            intData = stoi(str);
            
            isString = false;
            isInt = true;
        }
    };
    ~Value(){};

    string toString() {
        if (isNull) {
            return "";
        }
        else if (isInt) {
            return std::to_string(intData);
        } else if (isString) {
            return "'" + strData + "'";
        } else {
            fprintf(stderr, "Wrong with Value toString()\n");
            return "";
        }
    }
};

class Parser{
private:
    string query_str;
    Scanner scanner;
    
public:
    bool isCreateTableQuery;
    bool isInsertQuery;
    
    // memver of both CREATE TABLE and INSERT query
    string table_name;

    // member of CREATE TABLE query
    vector<Attribute> schema;

    // member of INSERT query
    bool orderSpecified;
    vector<string> orders;
    vector<Value> values;

    Parser(string query_str);
    bool Parse();

    bool Create_Table_Query();
    bool Read_Schema();
    bool Read_Attr_Def(Attribute& attr);
    void Print();

    bool Insert_Query();
    bool Read_Order_Specify();
    bool Read_Value();
};



#endif