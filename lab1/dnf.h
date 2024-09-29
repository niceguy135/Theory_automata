#pragma once

#include <vector>
#include <iostream>
#include <set>

#include "impl.h"

class DNF {
public:
    DNF(std::string str);

    void Minimize();
    void print(std::ostream &os) const;
    Impl& getImpl(int index);

private:
    std::vector<Impl> impls;
    std::vector<Impl> sdnf;
    int len_param=0;
};