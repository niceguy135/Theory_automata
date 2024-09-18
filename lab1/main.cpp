#include "implicant.h"
#include "utils.h"


int main() {
    std::ifstream scale("../scale.txt");
    std::ofstream mdnf("../mdnf.txt");
    std::string buff;
    std::vector<Implicant> func;

    uint16_t idx = 0;
    std::getline(scale, buff);

    // Заполнение импликантов
    std::transform(buff.begin(), buff.end(), back_inserter(func), [&idx](char x) {
        return Implicant(idx++, LogicValueFromChar(x));
    });

    uint8_t function_size = (uint8_t) log2(func.size());
    std::cout << "Загруженная функция " << (uint16_t) function_size << " аргументы:" << std::endl;

    std::cout << buff << std::endl;

    // debug print
    std::for_each(func.begin(), func.end(), [](Implicant& x) {
        std::cout << LogicValueToChar(x.GetValue());
    });

    std::cout << std::endl << "Таблица истинности для функции:" << std::endl;
    PrintVariables(std::cout, (uint8_t)function_size);

    std::for_each(func.begin(), func.end(), [&function_size](Implicant& impl) {
        impl.PrintImplicant(function_size);
    });

    std::cout << std::endl;
    // M1: отфильтруйте ложные значения. Считайте '-' как '1'
    std::vector<Implicant> m_1;

    std::remove_copy_if(func.begin(), func.end(), back_inserter(m_1), [](const Implicant& x) {
        return x.GetValue() == FALSE;
    });

    std::cout << "M1:" << std::endl;
    Implicant::PrintVector(m_1);

    std::cout << std::endl;

    // M2: патчинг значений
    std::cout << "M2:" << std::endl;

    std::vector<Implicant> m_2;
    Implicant::PatchVectors(m_1, m_2);
    Implicant::PrintVector(m_2);

    std::cout << std::endl;

    // M3: пропачим M2 один раз
    std::cout << "M3:" << std::endl;
    std::vector<Implicant> m_3;
    Implicant::PatchVectors(m_2, m_3);
    Implicant::PrintVector(m_3);
    std::cout << std::endl;
    uint16_t patched_implicants = 0;

    int tmp = 0;

    // M4:  патч M3 до тех пор, пока исправление не станет невозможным
    std::cout << "M4: " << std::endl;
    std::vector<Implicant> m_4;
    std::vector<Implicant> m_4_prev;

    std::transform(m_3.begin(), m_3.end(), back_inserter(m_4_prev), [](Implicant x) {
        x.SetPatched(false);
        return x;
    });

    do {
        // очистка M4, прежде чем что-либо делать
        m_4.clear();

        Implicant::PatchVectors(m_4_prev, m_4);
        // Очистка: устанавливаем m_4_prev, вычисляем исправленные импликатны
        m_4_prev.clear();

        std::transform(m_4.begin(), m_4.end(), back_inserter(m_4_prev), [](Implicant x) {
            x.SetPatched(false);
            return x;
        });

        // Удаление дубликатов

        std::vector<Implicant> tmpvec(m_4_prev);

        m_4_prev.clear();

        std::for_each(tmpvec.begin(), tmpvec.end(), [&tmpvec, &m_4_prev](Implicant x) {
            int count_source = accumulate(tmpvec.begin(), tmpvec.end(), 0, [&x](int acc, const Implicant& y) {
                return (x == y) ? acc + 1 : acc;
            });
            bool dest_contains = accumulate(m_4_prev.begin(), m_4_prev.end(), false, [&x](bool acc, const Implicant& y) {
                return acc || (x == y);
            });
            if (count_source == 1 || !dest_contains) {
                m_4_prev.push_back(x);
            }
        });

        patched_implicants = accumulate(m_4.begin(), m_4.end(), (uint16_t) 0, [](uint16_t acc, const Implicant &x) {
            return (x.WasPatched()) ? acc + 1 : acc;
        });
    } while(patched_implicants > 0 && (++tmp) < 10);

    Implicant::PrintVector(m_4);
    std::cout << "--------------------------" << "ТДНФ:" << "--------------------------"<< std::endl;
    PrintVariables(std::cout, (uint8_t) function_size);

    std::for_each(m_4.rbegin(), m_4.rend(), [&function_size](const Implicant& impl) {
        std::cout << impl.ImplicantToString(function_size) << std::endl;
    });
    std::cout << std::endl;
    
    
    std::cout << "--------------------------" << "МДНФ:" << "--------------------------"<< std::endl;
    Implicant::PrintTable(func, m_4, (uint8_t) function_size);

    // Очистка
    std::vector<Implicant> mdnf_source;
    std::remove_copy_if(func.begin(), func.end(), back_inserter(mdnf_source), [](const Implicant& x) {
        return (x.GetValue() == INDETERMINATE);
    });
 
    // Удалите избыточные импликанты ДНФ
    std::vector<Implicant> mdnf_m(m_4);
    std::vector<Implicant> mdnf_m_prev;
    std::vector<bool> can_be_removed;
    do {
        mdnf_m_prev.clear();
        can_be_removed.clear();
        std::copy(mdnf_m.begin(), mdnf_m.end(), back_inserter(mdnf_m_prev));

        std::transform(mdnf_m_prev.begin(), mdnf_m_prev.end(), back_inserter(can_be_removed), [&mdnf_m_prev, &mdnf_source, &func](const Implicant& x){
            std::vector<Implicant> other_implicants;
            std::remove_copy_if(mdnf_m_prev.begin(), mdnf_m_prev.end(), back_inserter(other_implicants), [&x](const Implicant& y) {
                return x == y;
            });

            // Значения функций, на которые распространяется x
            std::vector<Implicant> covered_by_x;
            std::remove_copy_if(func.begin(), func.end(), back_inserter(covered_by_x), [&x](const Implicant& y) {
                return ((y.GetIndex() & ~x.GetPatch()) != x.GetIndex()) || y.GetValue() != TRUE;
            });

            // Проверим, что охвачены ли все импликанты
            bool everything_is_covered = std::accumulate(covered_by_x.begin(), covered_by_x.end(), true, [&other_implicants](bool acc, const Implicant& y) {
                bool covered = std::accumulate(other_implicants.begin(), other_implicants.end(), false, [&y](bool acc, const Implicant& z) {
                    return acc || ((y.GetIndex() & ~z.GetPatch()) == z.GetIndex());
                });

                return acc && covered;
            });

            return everything_is_covered;
        });

        int imp_idx = 0;
        bool found = false;

        std::cout << "Может быть удалено: ";
        std::for_each(can_be_removed.begin(), can_be_removed.end(), [](bool x) {
            std::cout << x;
        });
        std::cout << std::endl;

        mdnf_m.clear();

        std::remove_copy_if(mdnf_m_prev.begin(), mdnf_m_prev.end(), back_inserter(mdnf_m), [&mdnf_m_prev, &mdnf_source, &func, &can_be_removed, &imp_idx, &found](const Implicant& x) {
            bool possible = can_be_removed[imp_idx++];

            if (possible && !found) {
                std::vector<Implicant> other_implicants;
                std::remove_copy_if(mdnf_m_prev.begin(), mdnf_m_prev.end(), back_inserter(other_implicants), [&x](const Implicant& y) {
                    return x == y;
                });

                std::vector<Implicant> covered_by_func;
                std::remove_copy_if(func.begin(), func.end(), back_inserter(covered_by_func), [&x](const Implicant& y) {
                    return y.GetValue() != TRUE;
                });

                bool everything_is_covered = std::accumulate(covered_by_func.begin(), covered_by_func.end(), true, [&other_implicants](bool acc, const Implicant& y) {
                    bool covered = std::accumulate(other_implicants.begin(), other_implicants.end(), false, [&y](bool acc, const Implicant& z) {
                        return acc || ((y.GetIndex() & ~z.GetPatch()) == z.GetIndex());
                    });

                    return acc && covered;
                });

                bool found_prev = found;

                if (everything_is_covered && !found) {
                    found = true;
                }

                return everything_is_covered && !found_prev;
            } else {
                return false;
            }
        });
    } while (std::accumulate(can_be_removed.begin(), can_be_removed.end(), false, [](bool acc, bool x) { return acc || x; }));

    std::cout << "Импликанты после удаления избыточных:" << std::endl;

    Implicant::PrintVector(mdnf_m);

    std::cout << std::setw(10) << " ";

    std::cout << std::endl;

    PrintVariables(std::cout, (uint8_t) function_size);
    PrintVariables(mdnf, (uint8_t) function_size);

    std::for_each(mdnf_m.rbegin(), mdnf_m.rend(), [&function_size, &mdnf](const Implicant& impl) {
        std::cout << impl.ImplicantToString(function_size) << std::endl;
        mdnf << impl.ImplicantToString(function_size) << std::endl;
    });

    Implicant::PrintTable(func, mdnf_m, (uint8_t) function_size);

    scale.close();
    mdnf.close();

    return 0;
}