#ifndef INDEX_STRUCT_HPP
#define INDEX_STRUCT_HPP

#include <unordered_map>
#include "parser.hpp"

namespace std {

    template <>
    struct hash<Value>
    {
        std::size_t operator()(const Value& v) const
        {
            using std::hash;
            using std::string;
            if (v.isInt) {
                return hash<int>()(v.intData);
            } else if (v.isString) {
                return hash<string>()(v.strData);
            } else {
                fprintf(stderr, "Error: The hashed value v is neither a int or a string\n");
                exit(EXIT_FAILURE);
            }
        }
    };

}

class Index_Struct
{
public:
    Index_Struct(){};
    ~Index_Struct(){};
    virtual void insert(const Value& v, const int row) {};
    virtual const std::vector<int>& satisfied_rows(CompareOP op, const Value& v) = 0;
    virtual const std::vector<int>& satisfied_rows(const Value& v, CompareOP op) = 0;
    virtual void print(){};
};

class Hash_Index_Struct: public Index_Struct
{
public:
    std::unordered_multimap<Value, int> m;
    Hash_Index_Struct(){};
    Hash_Index_Struct(const std::vector<std::vector<Value>>& tuples, int col){
        for(int i=0; i<tuples.size(); i++) {
            this->insert(tuples[i][col], i);
        }
        // this->print();
    };
    ~Hash_Index_Struct(){};
    void insert(const Value& v, const int row);
    const std::vector<int>& satisfied_rows(CompareOP op, const Value& v);
    const std::vector<int>& satisfied_rows(const Value& v, CompareOP op);
    void print();
};


class Tree_Index_Struct: public Index_Struct
{
public:
    std::multimap<Value, int> m;
    Tree_Index_Struct(){};
    Tree_Index_Struct(const std::vector<std::vector<Value>>& tuples, int col){
        for(int i=0; i<tuples.size(); i++) {
            this->insert(tuples[i][col], i);
        }
        // this->print();
    };
    ~Tree_Index_Struct(){};
    void insert(const Value& v, const int row);
    const std::vector<int>& satisfied_rows(CompareOP op, const Value& v);
    const std::vector<int>& satisfied_rows(const Value& v, CompareOP op);
    void print();
};

#endif