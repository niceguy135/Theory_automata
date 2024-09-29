#include <iostream>
#include <string>
#include <vector> 
#include <set>
#include <memory>

#include "impl.h"
#include "dnf.h"
#include "streamData.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<streamData> stream;
    std::string read;
    std::string write;
    
    if (argc >= 3) {
        read = argv[1];
        write = argv[2];
        stream = streamData::create<streamDataFile>(read, write);
    } else {
        stream = streamData::create<streamDataConsole>(std::cin, std::cout);
    }

    std::string str = stream->readData();
    DNF dnf(str);
    dnf.Minimize();
    stream->writeData(dnf);

	return 0;
}
