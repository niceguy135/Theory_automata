#pragma once

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <deque>
#include <unordered_set>

class Machine {
public:
    virtual ~Machine() = default;
};

class Mealy : public Machine {
public:
    Mealy(const std::string& state_path, const std::string& output_path);

    void prepareInputs(const std::string &path);
    void coddingStates();
    void generateDTriggerSndf(std::map<std::string, std::unordered_set<std::string>> &res);
    void generateOutputSndf(std::map<std::string, std::unordered_set<std::string>> &res);

    void printState();
    void printOutput();

    std::deque<std::string> format(std::map<std::string, std::unordered_set<std::string>> &res);

private:
    std::map<std::string, std::string> inputs;
    std::map<std::string, std::list<std::pair<std::string, std::string>>> transition_state;
    std::map<std::string, std::list<std::pair<std::string, std::string>>> transition_output;
    std::map<std::string, std::string> codeState;
    std::map<std::string, std::string> outputState;

    int countTriggers = 0;
};