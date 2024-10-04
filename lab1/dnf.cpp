#include "dnf.h"

#include <numeric>
#include <cmath>
#include <array>
#include <set>

DNF::DNF(std::string str) {
    for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '-') {
            impls.emplace_back(i);
        } else if (str[i] - '0' == 1) {
            impls.emplace_back(i);
            sdnf.emplace_back(i);
        }
    }
    len_param = log2(str.size());
}

void DNF::Minimize() {
    std::vector<Impl> tdnf;
    std::vector<Impl> mdnf;
    std::vector<Impl> impls_temp;

    while (impls.size() > 0) {
        for (int i = 0; i < impls.size(); ++i) {
            for (int j = i; j < impls.size(); ++j) {
                auto impl = Impl::patch(impls[i], impls[j]);
                if (impl.has_value()) {
                    impls_temp.push_back(impl.value());
                }
            }

            if (!impls[i].Pw) {
                tdnf.push_back(impls[i]);
            }
        }
        impls = std::move(impls_temp);
    }

    int n = tdnf.size();
    int m = sdnf.size();
    std::vector<std::vector<int>> table(n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if ((sdnf[j].Num & (~tdnf[i].P)) == tdnf[i].Num) {
                table[i].push_back(sdnf[j].Num);
            }
        }
    }


    auto tdnf_const = tdnf;

    std::set<int> test_extra;
    bool finish = false;
    std::vector<std::vector<Impl>> result;
    auto table_const = table;
    auto tdnf_const_var = tdnf;
    int i;
    
    for (int k = 0; k < n; ++k) {
        for (int t = 0; t < n; ++t) {
            while (!finish) {
                for (int i = t; i < n; ++i) {
                    test_extra.clear();

                    for (int j = 0; j < n; ++j) {
                        if (j != i) {
                            for (int k = 0; k < table[j].size(); ++k) {
                                test_extra.insert(table[j][k]);
                            }
                        }
                    }
                    if (test_extra.size() == m) {
                        table.erase(table.begin() + i);
                        tdnf.erase(tdnf.begin() + i);
                        --n;
                        finish = false;
                        break;
                    } else {
                        finish = true;
                    }
                }
                i = 0;
            }
        }
        result.push_back(tdnf);
        table = table_const;
        tdnf = tdnf;
    }

    impls = tdnf_const;
    for (auto& vec : result) {
        if (vec.size() < impls.size()) {
            impls = vec;
        }
    }

    for (auto& impl : impls) {
        unq.insert(impl);
    }
}

void DNF::print(std::ostream &os) const {
    for (int index = 0; index < len_param; ++index) {
        os << getVar(index);
    }
    os << std::endl;

    for (auto& impl : unq) {
        for (int j = 0; j < len_param; ++j) {
            if ((impl.P >> (len_param - j - 1)) % 2 == 1) {
                os << "-";
            } else {
                auto integere = ((impl.Num >> (len_param - j - 1)) % 2);
                if (integere == 1) {
                    os << "\033[1;32m" << integere << "\033[0m"; 
                } else {
                    os << "\033[1;31m" << integere << "\033[0m";
                }
            }
        }
        std::cout << std::endl;
    }
}

char DNF::getVar(int index) const {
    static std::array<char, 9> symbols{'x', 'y', 'z', 'u', 'a', 'b', 'c', 'd', 'g'};

    return symbols[index];
}