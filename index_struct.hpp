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
    ~Index_Struct();
    virtual void insert(const Value& v, const int row);
    // virtual pair<iter, iter> f (condition) {

    // }
};

class Hash_Index_Struct: public Index_Struct
{
public:
    std::unordered_multimap<Value, int> m;
    Hash_Index_Struct();
    ~Hash_Index_Struct();
    void insert(const Value& v, const int row) {
        m.insert(std::make_pair(v, row));
    }
};
