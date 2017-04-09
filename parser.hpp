#ifndef PARSER_HPP
#define PARSER_HPP

/* Parser.hpp */
#include "scanner.hpp"
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <regex>

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

    bool operator<(const Value& a) const {
        if (isNull || a.isNull) {
            if (isNull && a.isNull) {
                // then *this == a, so !(*this < a) and !(a < *this)
                return false;
            } else if (isNull == true && a.isNull == false) {
                return true;
            } else {
                // isNull == false && a.isNull == true
                return false;
            }
        } else if (isInt) {
            return (intData < a.intData);
        } else if (isString) {
            return (strData.compare(a.strData) < 0);
        } else {
            fprintf(stderr, "WTF!?? at Value operator < \n");
            return false;
        }
    }
    bool operator==(const Value& a) const {
        if (isNull != a.isNull) {
            return false;
        } else if (isNull && a.isNull) {
            return true;
        } else if (isInt) {
            return (intData == a.intData);
        } else if (isString) {
            return (strData == a.strData);
        }
        fprintf(stderr, "WTF!?? at Value operator == \n");
        return false;
    }
};
struct AttributeID {
    AttributeID(){};
    AttributeID(const string& attr_name): tableSpecified(false), attr_name(attr_name){};
    AttributeID(const string& tableID, const string& attr_name): tableSpecified(true), tableID(tableID), attr_name(attr_name){};
    bool tableSpecified;
    string tableID; // table name or table alias
    string attr_name;
};
enum class CompareOP {BIGGER, SMALLER, EQUAL, NOT_EQUAL};
enum class LogicalOP {AND, OR};
enum class AggreFunc {SUM, COUNT};
enum class CompareType {ATTRIBUTE, INT_CONST, STRING_CONST};
class Parser{
private:
    string query_str;
    Scanner scanner;
    
public:
    bool isCreateTableQuery;
    bool isInsertQuery;
    bool isSelectQuery;
    
    // memver of both CREATE TABLE and INSERT query
    string table_name;

    // member of CREATE TABLE query
    vector<Attribute> schema;

    // member of INSERT query
    bool orderSpecified;
    vector<string> orders;
    vector<Value> values;

    struct SelectedItem {
        bool isAggregation;
        AggreFunc aggreFunc;
        AttributeID attributeID;
        SelectedItem(const string& aggreFuncStr, const AttributeID& attributeID): attributeID(attributeID) {
            isAggregation = true;
            if (aggreFuncStr == "SUM") {
                aggreFunc = AggreFunc::SUM;
            } else if (aggreFuncStr == "COUNT") {
                aggreFunc = AggreFunc::COUNT;
            } else {
                fprintf(stderr, "WTF? aggreFuncStr %s doesn't make sence\n", aggreFuncStr.c_str());
            }
        }
        SelectedItem(const AttributeID& attributeID): isAggregation(false),  attributeID(attributeID){}
    };
    struct ComparePair {
        CompareType type1;
        CompareType type2;
        
        string str1, str2;
        int int1, int2;

        AttributeID attrID1;
        CompareOP op;
        AttributeID attrID2;

        ComparePair(const AttributeID& attrID1, CompareOP op, const AttributeID& attrID2):
                    attrID1(attrID1), op(op), attrID2(attrID2) {
            type1 = type2 = CompareType::ATTRIBUTE;
        }
        ComparePair(const AttributeID& attrID1, CompareOP op, const string& pair2):
                    attrID1(attrID1), op(op){
            type1 = CompareType::ATTRIBUTE;
            if (Parser::isStrString(pair2)) {
                type2 = CompareType::STRING_CONST;
                int2 = std::stoi(pair2);
            } else if (Parser::isIntString(pair2)) {
                type2 = CompareType::INT_CONST;
                str2 = pair2;
            } else {
                fprintf(stderr, "typeOfString wrong: no type for %s\n", pair2.c_str());
                exit(EXIT_FAILURE);
            }
        }
        ComparePair(const string& pair1, CompareOP op,const AttributeID& attrID2):
                    op(op), attrID2(attrID2) {
            type2 = CompareType::ATTRIBUTE;
            if (Parser::isStrString(pair1)) {
                type1 = CompareType::STRING_CONST;
                int1 = std::stoi(pair1);
            } else if (Parser::isIntString(pair1)) {
                type1 = CompareType::INT_CONST;
                str1 = pair1;
            } else {
                fprintf(stderr, "typeOfString wrong: no type for %s\n", pair1.c_str());
                exit(EXIT_FAILURE);
            }
        }
        ComparePair(const string& pair1, CompareOP op, const string& pair2): op(op) {
            if (Parser::isStrString(pair2)) {
                type2 = CompareType::STRING_CONST;
                int2 = std::stoi(pair2);
            } else if (Parser::isIntString(pair2)) {
                type2 = CompareType::INT_CONST;
                str2 = pair2;
            } else {
                fprintf(stderr, "typeOfString wrong: no type for %s\n", pair2.c_str());
                exit(EXIT_FAILURE);
            }
            if (Parser::isStrString(pair1)) {
                type1 = CompareType::STRING_CONST;
                int1 = std::stoi(pair1);
            } else if (Parser::isIntString(pair1)) {
                type1 = CompareType::INT_CONST;
                str1 = pair1;
            } else {
                fprintf(stderr, "typeOfString wrong: no type for %s\n", pair1.c_str());
                exit(EXIT_FAILURE);
            }
        }
    };
    
    struct SelectQueryData {
        vector<SelectedItem> selectedItems; 
        
        vector<ComparePair> comparePairs;
        LogicalOP logicalOP;

        map<string, string> aliasToTableName;
        vector<string> fromTables;
    };
    SelectQueryData* selectData;

    Parser(string query_str);
    bool Parse();
    bool Create_Table_Query();
    bool Read_Schema();
    bool Read_Attr_Def(Attribute& attr);
    void Print();

    bool Insert_Query();
    bool Read_Order_Specify();
    bool Read_Value();

    bool Select_Query();
    bool read_Selected_Item_Sequence();
    bool read_Selected_Item();
    bool read_AttrID(AttributeID& attrID);
    
    bool read_FromTable_Sequence();
    bool read_FromTable();
    
    bool read_Where_Clause();
    bool read_ComparePair(ComparePair& cmpPair);

    static bool validName(string& name);
    static bool isIntString(const string& str);
    static bool isStrString(const string& str);
};



#endif