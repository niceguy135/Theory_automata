#pragma once

#include <fstream>
#include <memory>
#include <functional>
#include <type_traits>

#include "dnf.h"

class streamDataFile;
class streamDataConsole;

class streamData {
public:
    virtual ~streamData() = default;

    virtual std::string readData() = 0;
    virtual void writeData(const DNF &dnf) = 0;

    template <typename streamType, typename ...Args>
    static std::unique_ptr<streamData> create(Args&& ...args) {
        return std::make_unique<streamType>(std::forward<Args>(args)...);
    }
};

class streamDataFile : public streamData {
public:
    streamDataFile(std::string& filenameRead,
                   std::string& filenameWrite);

    ~streamDataFile();

    std::string readData() override;
    void writeData(const DNF &dnf) override;

private:
    std::ifstream readStream;
    std::ofstream writeStream;
};

class streamDataConsole : public streamData {
public:
    streamDataConsole(std::istream &in, std::ostream &out);
    ~streamDataConsole() = default;

    std::string readData() override;
    void writeData(const DNF &dnf) override;

private:
    std::reference_wrapper<std::istream> readStream;
    std::reference_wrapper<std::ostream> writeStream;
};
