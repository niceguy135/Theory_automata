#include <iostream>
#include <string>
#include <vector> 
#include <set>
#include <memory>

#include "impl.h"
#include "dnf.h"
#include "streamData.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<streamDataFile> stream;
    std::string read;
    std::string write;

    read = argv[1];
    write = argv[2];
    stream = streamDataFile::create<streamDataFile>(read, write);

    std::string str = stream->readData();
    DNF dnf(str);
    dnf.Minimize();
    stream->writeData(dnf);

	return 0;
}
