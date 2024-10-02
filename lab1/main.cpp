#include <iostream>
#include <string>
#include <vector> 
#include <set>
#include <memory>
#include <thread>
#include <mutex> 

#include "impl.h"
#include "dnf.h"
#include "streamData.h"

void generation(const std::string &table, size_t index, std::vector<std::string> &result) {
    if (index >= table.size()) {
        result.push_back(table);
        return;
    }

    if (table[index] != '-') {
       generation(table, index + 1, result); 
    } else {
        std::string zero = table;
        std::string one = table;

        zero[index] = '0';
        one[index] = '1';

        generation(zero, index + 1, result);
        generation(one, index + 1, result);
    }
}

std::vector<std::string> AllCombination(std::string &table) {
    std::vector<std::string> result;
    generation(table, 0, result);
    return result;
}

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

#ifdef MULTITHREAD
    auto combinations = AllCombination(str);

    std::mutex oibound;
    int count = 1;
    std::vector<std::thread> threads;
    for (auto& table : combinations) {
        threads.emplace_back([&stream, &table, &oibound, &count]() -> void {
            DNF dnf(table);
            dnf.Minimize();

            std::lock_guard<std::mutex> lock(oibound);
#ifdef PERFECT_OUT
            std::cout << "\033[1;33m" << "МДНФ №" << count << "\033[0m" << std::endl;
            std::cout << "\033[1;33m" << "Изначальная таблица: " << "\033[0m" << "\033[1;34m" << table <<"\033[0m" << std::endl; 
#endif
            stream->writeData(dnf);
            ++count;
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
#else
    DNF dnf(str);
    dnf.Minimize();
    stream->writeData(dnf);
#endif

	return 0;
}
