#include "machine.h"

#include <fstream>
#include <cmath>
#include <sstream>

Mealy::Mealy(const std::string& state_path, const std::string& output_path) {
    prepareInputs(state_path);
    
    std::ifstream file;
    file.open(state_path);
    if (!file.is_open()) {
        throw std::logic_error("failed open read file");
    }

    std::string state("S");
    std::string line;
    
    auto it = inputs.begin();
    while (std::getline(file, line)) {
        for (int index = 0; index < line.size(); ++index) {
            transition_state[state + std::to_string(index + 1)].emplace_back(state + std::string(1, line[index]), it->first);
        }
        it = std::next(it);
    }
    file.close();

    file.open(output_path);
    if (!file.is_open()) {
        throw std::logic_error("failed open read file");
    }

    it = inputs.begin();
    std::string output(2, 'y');
    while (std::getline(file, line)) {
        for (int index = 0; index < line.size(); ++index) {
            output[1] = line[index];
            transition_output[state + std::to_string(index + 1)].emplace_back(output, it->first);
        }
        it = std::next(it);
    }

    file.close();

    // максимальное нужное значение регистров
    countTriggers = std::ceil(std::log2(transition_state.size()));
    std::cout << std::endl << "Количество триггеров: " << countTriggers << std::endl;
}

void Mealy::printState() {
    std::cout << "   ";
    for (auto it = transition_state.begin(); it != transition_state.end(); ++it) {
        std::cout << it->first << " ";
    }
    std::cout << std::endl;

    for (auto state = transition_state.begin(); state != transition_state.end(); ++state) {
        bool first = true;
        for (auto it = state->second.begin(); it != state->second.end(); ++it) {
            if (first) {
                std::cout << it->second << " ";
            }
            std::cout << it->first << " ";
            first = false;
        }
        std::cout << std::endl;
    }
}

void Mealy::printOutput() {
    std::cout << "   ";
    for (auto it = transition_output.begin(); it != transition_output.end(); ++it) {
        std::cout << it->first << " ";
    }

    std::cout << std::endl;
    for (auto state = transition_output.begin(); state != transition_output.end(); ++state) {
        bool first = true;
        for (auto it = state->second.begin(); it != state->second.end(); ++it) {
            if (first) {
                std::cout << it->second << " ";
            }
            std::cout << it->first << " ";
            first = false;
        }
        std::cout << std::endl;
    }
}

void Mealy::prepareInputs(const std::string &path) {
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        throw std::logic_error("failed open read file");
    }

    int count_lines = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++count_lines;
    }
    file.clear();
    file.seekg(0, std::ios::beg);
    
    int count = 0;
    std::string input(2, 'x');

    while (std::getline(file, line)) {
        input[1] = '0' + count;
        std::bitset<32> binaryCode(count);
        inputs[input] = binaryCode.to_string().substr(32 - std::ceil(std::log2(count_lines)), std::ceil(std::log2(count_lines)));
        ++count;
    }

    for (auto& el : inputs) {
        std::cout << el.first << " " << el.second << std::endl;
    }

    file.close();
}

void Mealy::coddingStates() {
    int code = std::pow(2, countTriggers) - 1;
    auto it = transition_state.begin();
    while (codeState.size() != transition_state.size() && it != transition_state.end() && code >= 0) {
        std::bitset<32> binaryCode(code);
        codeState[(*it).first] = binaryCode.to_string().substr(32 - countTriggers, countTriggers);
        --code;
        it = std::next(it);
    }

    for (auto it = codeState.begin(); it != codeState.end(); ++it) {
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }

    std::string output(2, 'y');
    code = std::pow(2, countTriggers) - 1;
    while (outputState.size() != transition_output.size()) {
        std::bitset<32> binaryCode(code);
        output[1] = '0' + code;
        outputState[output] = binaryCode.to_string().substr(32 - countTriggers, countTriggers);
        --code;
    }

    for (auto& it : outputState) {
        std::cout << it.first << " " << it.second << std::endl;
    }
}

void Mealy::generateDTriggerSndf(std::map<std::string, std::unordered_set<std::string>> &res) {
    std::cout << std::endl << "СДНФ для функций возбуждения D-триггеров" << std::endl;

    for (int index = 0; index < countTriggers; ++index) {
        std::string trigger("D" + std::to_string(index));
        std::cout << trigger << " = ";
        std::string terms;
        bool firstTerm = true;

        // ищем где по позиции index равен 1
        for (auto state = transition_state.begin(); state != transition_state.end(); ++state) {
            for (auto transit = state->second.begin(); transit != state->second.end(); ++transit) {
                auto code = codeState.find(transit->first);
                if (code == codeState.end()) {
                    if (transit->first == "S-") {
                        continue;
                    }
                    throw std::logic_error("Undefined state");
                }

                if (code->second[index] == '1') {
                    if (!firstTerm) {
                        std::cout << "v";
                        terms += "v";
                    }
                    std::cout << state->first << transit->second;
                    // кодированное состояние
                    terms += code->second;
                    // кодированный вход
                    terms += inputs.find(transit->second)->second;
                    firstTerm = false;
                }
            }
        }
        std::cout << std::endl;

        std::istringstream ss(terms);
        std::string term;

        while (std::getline(ss, term, 'v')) {
            res[trigger].insert(term);
        }
    }
}

void Mealy::generateOutputSndf(std::map<std::string, std::unordered_set<std::string>> &res) {
    std::cout << std::endl << "СДНФ для выходов" << std::endl;

    for (int index = 0; index < countTriggers; ++index) {
        std::string output("y" + std::to_string(index));
        std::cout << output << " = ";
        std::string terms;
        bool firstTerm = true;

        for (auto state = transition_output.begin(); state != transition_output.end(); ++state) {
            for (auto transit = state->second.begin(); transit != state->second.end(); ++transit) {
                auto code = outputState.find(transit->first);
                if (code == outputState.end()) {
                    if (transit->first == "y-") {
                        continue;
                    }
                    throw std::logic_error("Undefined state");
                }

                if (code->second[index] == '1') { 
                    if (!firstTerm) {
                        std::cout << "v";
                        terms += "v";
                    }
                    std::cout << state->first << transit->second;
                    // кодированное состояние
                    terms += code->second;
                    // кодированный вход
                    terms += inputs.find(transit->second)->second;
                    firstTerm = false;
                }
            }
        }

        std::cout << std::endl;

        std::istringstream ss(terms);
        std::string term;

        while (std::getline(ss, term, 'v')) {
            res[output].insert(term);
        }
    }
}


std::deque<std::string> Mealy::format(std::map<std::string, std::unordered_set<std::string>> &res) {
    std::deque<std::string> result;
    int countBit = res.begin()->second.begin()->size();
    
    for (auto terms = res.begin(); terms != res.end(); ++terms) {
        std::string result_temp;
        for (int num = 0; num < (1 << countBit); ++num) {
            std::string table(countBit, '0');

            for (int i = 0; i < countBit; ++i) {
                if (num & (1 << i)) {
                    table[countBit - 1 - i] = '1';
                }
            }

            if (terms->second.find(table) != terms->second.end()) {
                result_temp += '1';
            } else {
                result_temp += '0';
            }
        }
        result.push_back(result_temp);
    }

    // while (!result.empty()) {
    //     std::cout << result.front() << std::endl;
    //     result.pop_front();
    // }

    return result;
}
