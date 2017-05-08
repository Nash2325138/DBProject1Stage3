#ifndef PARSER_HPP
#define PARSER_HPP

/* Parser.hpp */
#include "scanner.hpp"
#include "color.h"
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
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & name;
        ar & type;
        ar & isPrimaryKey;
        ar & char_len;
    }
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

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & isNull;
        ar & isInt;
        ar & isString;
        ar & intData;
        ar & strData;
    }

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

    string toString() const{
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
    bool smaller(const Value& a) const{
        // if (isNull) {};
        if (isInt) {
            return (intData < a.intData);
        } else if (isString) {
            return (strData < a.strData);
        } else {
            fprintf(stderr, "Not int or string(SMALLER)\n");
            exit(EXIT_FAILURE);
            return false;
        }
    }
    bool bigger(const Value& a) const{
        if (isInt) {
            return (intData > a.intData);
        } else if (isString) {
            return (strData > a.strData);
        } else {
            fprintf(stderr, "Not int or string(BIGGER)\n");
            exit(EXIT_FAILURE);
            return false;
        }
    }
    bool equal(const Value& a) const{
        if (isInt) {
            return (intData == a.intData);
        } else if (isString) {
            return (strData == a.strData);
        } else {
            fprintf(stderr, "Not int or string(EQUAL)\n");
            exit(EXIT_FAILURE);
            return false;
        }
    }
    bool isTrue() const{
        if (isNull) return false;
        if (isInt) return (intData != 0);
        if (isString) return (!strData.empty());
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
    string toString() {
        if (tableSpecified) {
            return tableID + "." + attr_name;
        } else {
            return attr_name;
        }
    }
};
enum class CompareOP {BIGGER=0, SMALLER=1, EQUAL=2, NOT_EQUAL=3, OP_EMPTY=4};
enum class LogicalOP {AND, OR};
// enum class AggreFunc {SUM, COUNT};
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

    // items after SELECT   
    struct SelectedItem {
        bool isAggregation;
        string aggreFuncStr;
        AttributeID attributeID;
        SelectedItem(const string& aggreFuncStr, const AttributeID& attributeID): aggreFuncStr(aggreFuncStr), attributeID(attributeID) {
            isAggregation = true;
            if (aggreFuncStr != "sum" and aggreFuncStr != "count") {
                fprintf(stderr, "WTF? aggreFuncStr %s doesn't make sence\n", aggreFuncStr.c_str());
                exit(EXIT_FAILURE);
            }
        }
        SelectedItem(const AttributeID& attributeID): isAggregation(false),  attributeID(attributeID){}
        string toString() {
            static char buffer[1000];
            if (isAggregation) {
                strcpy(buffer, (aggreFuncStr + "(").c_str());
                strcat(buffer, attributeID.toString().c_str());
                strcat(buffer, ")");
            } else {
                strcpy(buffer, attributeID.toString().c_str());
            }
            return buffer;
        }
    };

    // items after WHERE
    struct ComparePair {
        CompareType type1;
        CompareType type2;
        
        // string str1, str2;
        // int int1, int2;
        Value v1, v2;

        AttributeID attrID1;
        CompareOP op;
        AttributeID attrID2;
        inline void fillPartPair(const string& part, Value& v, CompareType& type) {
            if (Parser::isStrString(part)) {
                type = CompareType::STRING_CONST;
                v = Value(part);
            } else if (Parser::isIntString(part)) {
                type = CompareType::INT_CONST;
                v = Value(part);
            } else {
                fprintf(stderr, "typeOfString wrong: no type for %s\n", part.c_str());
                exit(EXIT_FAILURE);
            }
        }
        ComparePair(){};
        ComparePair(const AttributeID& attrID1, CompareOP op, const AttributeID& attrID2):
                    attrID1(attrID1), op(op), attrID2(attrID2) {
            type1 = type2 = CompareType::ATTRIBUTE;
        }
        ComparePair(const AttributeID& attrID1, CompareOP op, const string& part2):
                    attrID1(attrID1), op(op){
            type1 = CompareType::ATTRIBUTE;
            this->fillPartPair(part2, v2, type2);
        }
        ComparePair(const string& part1, CompareOP op,const AttributeID& attrID2):
                    op(op), attrID2(attrID2) {
            type2 = CompareType::ATTRIBUTE;
            this->fillPartPair(part1, v1, type1);
        }
        ComparePair(const string& part1, CompareOP op, const string& part2): op(op) {
            this->fillPartPair(part1, v1, type1);
            this->fillPartPair(part2, v2, type2);
        }
        ComparePair(const string& part1): op(CompareOP::OP_EMPTY) {
            this->fillPartPair(part1, v1, type1);
        }
        ComparePair(const AttributeID& attrID1): attrID1(attrID1), op(CompareOP::OP_EMPTY) {
            type1 = CompareType::ATTRIBUTE;
        }
        string partToString(CompareType type, Value& v, AttributeID attrID) {
            char buffer[1000];
            if (type == CompareType::INT_CONST) {
                sprintf(buffer, "Type::INT_CONST, data: %d", v.intData);
            } else if (type == CompareType::STRING_CONST) {
                sprintf(buffer, "Type::STRING_CONST, data: %s", v.strData.c_str());
            } else if (type == CompareType::ATTRIBUTE) {
                sprintf(buffer, "Type::ATTRIBUTE, data: %s", attrID.toString().c_str());
            } else {
                fprintf(stderr, "illegal type of part of comparePair\n");
                exit(EXIT_FAILURE);
                return "";
            }
            return buffer;
        }
        string toString() {
            char buffer[2000];
            strcpy(buffer, partToString(type1, v1, attrID1).c_str());
            sprintf(buffer+strlen(buffer), "\t, opcode: %d, ", op);
            strcat(buffer, partToString(type2, v2, attrID2).c_str());
            return buffer;
        }
    };
    
    struct SelectQueryData {
        vector<SelectedItem> selectedItems; 
        
        vector<ComparePair> comparePairs;
        LogicalOP logicalOP;

        map<string, string> aliasToTableName;
        vector<string> fromTables;
        string toString() {
            char buffer[40000];
            
            strcpy(buffer, YELLOW);
            strcat(buffer, "selectedItems:\n");
            strcat(buffer, NONE);
            for (auto& item: selectedItems) {
                strcat(strcat(buffer, item.toString().c_str()), "\n");
            }

            strcat(buffer, YELLOW);
            strcat(buffer, "\nfromTables:\n");
            strcat(buffer, NONE);
            for (auto& table: fromTables) {
                strcat(strcat(buffer, table.c_str()), "\n");
            }

            strcat(buffer, YELLOW);
            strcat(buffer, "\naliasToTableName:\n");
            strcat(buffer, NONE);
            for (auto& aliasPair: aliasToTableName) {
                strcat(strcat(strcat(buffer, aliasPair.first.c_str()), " -> "), aliasPair.second.c_str());
                strcat(buffer, "\n");
            }

            if (comparePairs.size() > 0) {
                strcat(strcat(buffer, "\ncomparePair1:\n"), comparePairs[0].toString().c_str()); 
                if (comparePairs.size() > 1) {
                    strcat(strcat(buffer, "\ncomparePair2:\n"), comparePairs[1].toString().c_str()); 
                }
            }
            strcat(buffer, "\n");
            return buffer;

        }
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
    static bool validSelectName(string& name);
    static bool isIntString(const string& str);
    static bool isStrString(const string& str);
};



#endif