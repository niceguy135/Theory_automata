#include "impl.h"

#include <stdexcept>
#include <iostream>

Impl::Impl(int num) 
    : Num(num)
    , Ind(Impl::count(num))
    , P(0)
    , Pw(false)
    , Inf(false)
{}

int Impl::count(int num) {
    int cnt = 0;
    while (num) {
        cnt += num % 2;
        num /= 2;
    }

    return cnt;
}

std::optional<Impl> Impl::patch(Impl &lhs, Impl &rhs) {
    if (lhs.Num < rhs.Num
        && lhs.P == rhs.P
        && ((rhs.Ind - lhs.Ind) == 1)
        && (Impl::count(rhs.Num - lhs.Num) == 1)) {
        lhs.Pw = 1;
        rhs.Pw = 1;

        Impl other(lhs.Num);
        other.P = (rhs.Num - lhs.Num) + lhs.P;
        
        return other;
    }

    return std::nullopt;
}