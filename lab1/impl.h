#pragma once

#include <optional>

class Impl {
public:
    Impl(int num);

    static int count(int num);
    static std::optional<Impl> patch(Impl &lhs, Impl &rhs);

public:
    int Num;
    int Ind;
    int P;
    bool Pw;
    bool Inf;
};