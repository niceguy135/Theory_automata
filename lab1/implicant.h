#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <set>

enum LogicValue {
    FALSE,
    TRUE,
    INDETERMINATE
};

class Implicant {
public:
    Implicant(uint16_t index, LogicValue value);
    Implicant(const Implicant &other);

    uint16_t GetIndex() const;
    LogicValue GetValue() const;
    uint16_t GetPopcount() const;
    uint16_t GetPatch() const;
    bool WasPatched() const;

    bool CanPatch(const Implicant& other) const;
    uint16_t GetDiffPopcount(const Implicant& other) const;

    bool operator==(const Implicant& other) const;
    bool operator!=(const Implicant& other) const;

    Implicant Patch(Implicant &other);
    void SetPatched(bool value);

    void PrintImplicant(uint8_t count) const;
    std::string ImplicantStringRepr() const;
    std::string ImplicantToString(uint8_t size) const;

    static void PrintVector(std::vector<Implicant>& vec);
    static void PrintSet(std::set<Implicant>& vec);
    static void PatchVectors(std::vector<Implicant>& source, std::vector<Implicant>& target, bool verbose = false);
    static void PrintTable(std::vector<Implicant> func, std::vector<Implicant> dnf, const uint8_t& func_size);
private:
    Implicant(const Implicant& other, uint16_t patch);

private:
    uint16_t index_;
    LogicValue value_;
    bool was_patched_;
    uint16_t patch_;
};
