#include "index_struct.hpp"

void Hash_Index_Struct::insert(const Value& v, const int row) {
    m.insert(std::make_pair(v, row));
}

const std::vector<int>& Hash_Index_Struct::satisfied_rows(CompareOP op, const Value& v) {
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
        auto iter_pair = m.equal_range(v);
        for (auto it = iter_pair.first; it != iter_pair.second; it++) {
            ret.push_back(it->second);
        }
    } else if (op == CompareOP::NOT_EQUAL) {
        for (auto& p:m)
            if (!p.first.equal(v))
                ret.push_back(p.second);
    } else {
        assert(false);
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

const std::vector<int>& Hash_Index_Struct::satisfied_rows(const Value& v, CompareOP op) {
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
        auto iter_pair = m.equal_range(v);
        for (auto it = iter_pair.first; it != iter_pair.second; it++) {
            ret.push_back(it->second);
        }
    } else if (op == CompareOP::NOT_EQUAL) {
        for (auto& p:m)
            if (!v.equal(p.first))
                ret.push_back(p.second);
    } else {
        assert(false);
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

void Tree_Index_Struct::insert(const Value& v, const int row) {
    m.insert(std::make_pair(v, row));
}

const std::vector<int>& Tree_Index_Struct::satisfied_rows(CompareOP op, const Value& v) {
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
        auto iter_pair = m.equal_range(v);
        for (auto it = iter_pair.first; it != iter_pair.second; it++) {
            ret.push_back(it->second);
        }
    } else if (op == CompareOP::NOT_EQUAL) {
        for (auto& p:m)
            if (!p.first.equal(v))
                ret.push_back(p.second);
    } else {
        assert(false);
    }
    return ret;
}

const std::vector<int>& Tree_Index_Struct::satisfied_rows(const Value& v, CompareOP op) {
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
        auto iter_pair = m.equal_range(v);
        for (auto it = iter_pair.first; it != iter_pair.second; it++) {
            ret.push_back(it->second);
        }
    } else if (op == CompareOP::NOT_EQUAL) {
        for (auto& p:m)
            if (!v.equal(p.first))
                ret.push_back(p.second);
    } else {
        assert(false);
    }
    return ret;
}

void Hash_Index_Struct::print() {
    printf("Hash_Index_Struct:\n");
    for (auto p : m) {
        printf("Value: %s, row: %d\n", p.first.toString().c_str(), p.second);
    }
    printf("\n");
}
void Tree_Index_Struct::print() {
    printf("Tree_Index_Struct:\n");
    for (auto p : m) {
        printf("Value: %s, row: %d\n", p.first.toString().c_str(), p.second);
    }
    printf("\n");
}