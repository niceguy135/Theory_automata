#pragma once

#include <fstream>
#include <memory>
#include <functional>
#include <type_traits>

#include "dnf.h"

class streamDataFile{
public:
    streamDataFile(std::string& filenameRead,
                   std::string& filenameWrite);

    ~streamDataFile();

    std::string readData();
    void writeData(const DNF &dnf);

    template <typename streamType, typename ...Args>
    static std::unique_ptr<streamDataFile> create(Args&& ...args) {
        return std::make_unique<streamType>(std::forward<Args>(args)...);
    }

private:
    std::ifstream readStream;
    std::ofstream writeStream;
};

