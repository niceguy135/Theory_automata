#pragma once

#include <optional>

class Implicant {
public:
    Implicant(int num);

    static std::optional<Implicant> patch(Implicant &lhs, Implicant &rhs);
    static bool check(Implicant &lhs, Implicant &rhs);

public:
    int Num;
    int Ind;
    int P;
    bool Pw;
    bool Inf;
    static int count(int num);
};