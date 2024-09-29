#include "streamData.h"

#include <stdexcept>

streamDataFile::streamDataFile(std::string& filenameRead,
                           std::string& filenameWrite) {
    readStream.open(filenameRead);
    if (!readStream.is_open()) {
        throw std::logic_error("failed open read file");
    }

    writeStream.open(filenameWrite);
    if (!writeStream.is_open()) {
        throw std::logic_error("failed open write file");
    }
}

streamDataFile::~streamDataFile() {
    readStream.close();
    writeStream.close();
}

std::string streamDataFile::readData() {
    std::string line;

    std::getline(readStream, line);

    return line;
}

void streamDataFile::writeData(const DNF &dnf) {
    dnf.print(writeStream);
}

streamDataConsole::streamDataConsole(std::istream &in, std::ostream &out)
    : readStream(in)
    , writeStream(out)
{}

std::string streamDataConsole::readData() {
    std::string line;

    readStream.get() >> line;

    return line;
}

void streamDataConsole::writeData(const DNF &dnf) {
    dnf.print(writeStream.get());
}