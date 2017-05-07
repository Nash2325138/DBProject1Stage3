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
    Index_Struct();
    Index_Struct(const std::vector<std::vector<Value>>& tuples, int col) {
        for(int i=0; i<tuples.size(); i++) {
            this->insert(tuples[i][col], i);
        }
    }
    ~Index_Struct();
    virtual void insert(const Value& v, const int row);
    virtual const std::vector<int>& fullfill_rows(CompareOP op, const Value& v);
    virtual const std::vector<int>& fullfill_rows(const Value& v, CompareOP op);
};

class Hash_Index_Struct: public Index_Struct
{
public:
    std::unordered_multimap<Value, int> m;
    Hash_Index_Struct();
    Hash_Index_Struct(const std::vector<std::vector<Value>>& tuples, int col):Index_Struct(tuples, col) {};
    ~Hash_Index_Struct();
    void insert(const Value& v, const int row) {
        m.insert(std::make_pair(v, row));
    }
    virtual const std::vector<int>& fullfill_rows(CompareOP op, const Value& v) {
        static std::vector<int> ret;
        ret.clear();
        // hash map is UNORDERED, so whatever the op is, we need to iterator over all m
        if (op == CompareOP::OP_EMPTY) {
            for (auto& p:m)
                if (p.first.isTrue())
                    ret.push_back(p.second);
        } else if (op == CompareOP::BIGGER) {
            for (auto& p:m)
                if (p.first.bigger(v))
                    ret.push_back(p.second);
        } else if (op == CompareOP::SMALLER) {
            for (auto& p:m)
                if (p.first.smaller(v))
                    ret.push_back(p.second);
        } else if (op == CompareOP::EQUAL) {
            for (auto& p:m)
                if (p.first.equal(v))
                    ret.push_back(p.second);
        } else if (op == CompareOP::NOT_EQUAL) {
            for (auto& p:m)
                if (!p.first.equal(v))
                    ret.push_back(p.second);
        } else {
        }
        return ret;
    }
    virtual const std::vector<int>& fullfill_rows(const Value& v, CompareOP op) {
        static std::vector<int> ret;
        ret.clear();
        // hash map is UNORDERED, so whatever the op is, we need to iterator over all m
        if (op == CompareOP::OP_EMPTY) {
            for (auto& p:m)
                if (v.isTrue())
                    ret.push_back(p.second);
        } else if (op == CompareOP::BIGGER) {
            for (auto& p:m)
                if (v.bigger(p.first))
                    ret.push_back(p.second);
        } else if (op == CompareOP::SMALLER) {
            for (auto& p:m)
                if (v.smaller(p.first))
                    ret.push_back(p.second);
        } else if (op == CompareOP::EQUAL) {
            for (auto& p:m)
                if (v.equal(p.first))
                    ret.push_back(p.second);
        } else if (op == CompareOP::NOT_EQUAL) {
            for (auto& p:m)
                if (!v.equal(p.first))
                    ret.push_back(p.second);
        } else {
        }
        return ret;
    }
};
