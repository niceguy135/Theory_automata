#include "dnf.h"

#include <numeric>
#include <cmath>

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

    bool finish = false;

    while (!finish) {
        for (int i = 0; i < n; ++i) {
            std::set<int> tmp;

            for (int j = 0; j < n; ++j) {
                if (j != i) {
                    for (int k = 0; k < table[j].size(); ++k) {
                        tmp.insert(table[j][k]);
                    }
                }
            }
            if (tmp.size() == m) {
                table.erase(table.begin() + i);
                tdnf.erase(tdnf.begin() + i);
                --n;
                finish = false;
                break;
            } else {
                finish = true;
            }
        }
    }

    impls = tdnf;
}

void DNF::print(std::ostream &os) const {
    for (int i = 0; i < impls.size(); ++i) {
        for (int j = 0; j < len_param; ++j) {
            if ((impls[i].P >> (len_param - j - 1)) % 2 == 1) {
                os << "-";
            } else {
                os << ((impls[i].Num >> (len_param - j - 1)) % 2);
            }
        }
        os << std::endl;
    }
}

Impl& DNF::getImpl(int index) {
    return impls[index];
}