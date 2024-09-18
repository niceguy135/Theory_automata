#include "utils.h"

uint16_t CountOnes16(uint16_t input) {
    return __builtin_popcount(input);
}

LogicValue LogicValueFromChar(char input) {
    switch (input) {
        case '0': return FALSE;
        case '1': return TRUE;
        case '-': return INDETERMINATE;
        default: {
            const std::string error = (std::string)"Invalid DNF value: " + input;
            throw std::runtime_error(error);
        }
    }
}

char LogicValueToChar(LogicValue input) {
    switch (input) {
        case FALSE: return '0';
        case TRUE: return '1';
        case INDETERMINATE: return '-';
        default: throw 1;
    }
}

void PrintVariables(std::ostream& stream, uint8_t count ) {
    for (uint8_t i = 0; i < count; i++) {
        stream << VARIABLE_NAMES[i];
    }
    stream << std::endl;
}
