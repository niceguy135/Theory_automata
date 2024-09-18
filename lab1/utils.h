#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <set>

#include "implicant.h"

const char VARIABLE_NAMES[16] = {
        'x', 'y', 'z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h','i', 'u', 'v', 'p', 'q'
};

uint16_t CountOnes16(uint16_t input);
LogicValue LogicValueFromChar(char input);
char LogicValueToChar(LogicValue input);
void PrintVariables(std::ostream& stream, uint8_t count );