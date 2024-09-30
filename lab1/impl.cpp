#include "impl.h"

#include <stdexcept>
#include <iostream>

Implicant::Implicant(int num) 
    : Num(num), Ind(Implicant::count(num))
    , P(0), Pw(false), Inf(false)
{}

int Implicant::count(int num) {
    int cnt = 0;
    while (num) {
        cnt += num % 2;
        num /= 2;
    }

    return cnt;
}

bool Implicant::check(Implicant &lhs, Implicant &rhs){
    bool check_1 = lhs.Num < rhs.Num;
    bool check_2 = lhs.P == rhs.P;
    bool check_3 = ((rhs.Ind - lhs.Ind) == 1);
    bool check_4 = (Implicant::count(rhs.Num - lhs.Num) == 1);

    return check_1 && check_2 && check_3 && check_4;
}

std::optional<Implicant> Implicant::patch(Implicant &lhs, Implicant &rhs) {
    if (Implicant::check(lhs, rhs)) {
        lhs.Pw = 1;
        rhs.Pw = 1;

        Implicant other(lhs.Num);
        other.P = (rhs.Num - lhs.Num) + lhs.P;
        
        return other;
    }

    return std::nullopt;
}
