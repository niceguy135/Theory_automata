#pragma once

#include <vector>
#include <iostream>
#include <utility>

#include "impl.h"

class DNF {
public:
    DNF(std::string str);

    void Minimize();
    void print(std::ostream &os) const;

private:
    char getVar(int index) const;

private:
    std::vector<Impl> impls;
    std::vector<Impl> sdnf;
    int len_param = 0;
};