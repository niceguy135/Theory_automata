#pragma once

#include <vector>
#include <iostream>
#include <utility>
#include <unordered_set>

#include "impl.h"


struct hashImpl {
    std::size_t operator()(const Impl &impl) const {
        std::size_t h1 = std::hash<int>()(impl.Num);
        std::size_t h2 = std::hash<int>()(impl.P);

        return h1 ^ (h2 << 1);
    }
};

struct  ImplEq {
    bool operator()(const Impl &lhs, const Impl &rhs) const {
        return lhs == rhs;
    }
};

class DNF {
public:
    DNF(std::string str);

    void Minimize();
    void print(std::ostream &os) const;

private:
    char getVar(int index) const;

private:
    std::unordered_set<Impl, hashImpl, ImplEq> unq;
    std::vector<Impl> impls;
    std::vector<Impl> sdnf;
    int len_param = 0;
};