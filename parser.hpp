/* Parser.hpp */
#include "scanner.hpp"
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

class Parser{
private:
    string query_str;
    Scanner scanner;
    bool isCreateTableQuery;
    bool isInsertQuery;
    
    // member of CREATE TABLE query
    string table_name;
    vector<Attribute> schema;

    // member of INSERT query
    bool orderSpecified;
    vector<string> order;
    vector<string> values;
    
public:
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


