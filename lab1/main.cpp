#include <iostream>
#include <string> 
#include <set>
#include <memory>
#include <thread>
#include <deque>
#include <mutex> 

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

#include "impl.h"
#include "dnf.h"
#include "streamData.h"

void generation(const std::string &table, size_t index, std::deque<std::string> &result, int &count) {
    if (index >= table.size()) {
        result.push_back(table);
        return;
    }

    if (table[index] != '-') {
       generation(table, index + 1, result, ++count);
    } else {
        std::string zero = table;
        std::string one = table;

        zero[index] = '0';
        one[index] = '1';

        generation(zero, index + 1, result, ++count);
        generation(one, index + 1, result, ++count);
    }
}

std::deque<std::string> AllCombination(std::string &table, int &count) {
    std::deque<std::string> result;
    generation(table, 0, result, count);
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

    int cnt_combi = 0;
    auto combinations = AllCombination(str, cnt_combi);

// 101011-0101-01-1-011-1-1-01-0-10-1010011100-010010-0111100110
// no_thread 17,420
// threads 11,121

// 101011-0101--01-1-011-1-1-01-0-10-1010011100-010010-0111100110
// no_thread 55,408
// threads 36,915

    if (cnt_combi > 128) {
        std::mutex iobound;
        boost::asio::thread_pool pool(std::thread::hardware_concurrency());
        int count = 0;
        for (auto& table : combinations) {
            boost::asio::post(pool, [&iobound, &stream, &count, &table]() -> void {
                DNF dnf(table);
                dnf.Minimize();

                std::lock_guard<std::mutex> lock(iobound);
    #ifdef PERFECT_OUT
                std::cout << "\033[1;33m" << "МДНФ №" << count << "\033[0m" << std::endl;
                std::cout << "\033[1;33m" << "Изначальная таблица: " << "\033[0m" << "\033[1;34m" << table <<"\033[0m" << std::endl;
                ++count;
    #endif
                stream->writeData(dnf);
            });
        }

        pool.join();
    } else {
        int count = 0;
        for (auto &table : combinations) {
            DNF dnf(table);
            dnf.Minimize();
    #ifdef PERFECT_OUT
            std::cout << "\033[1;33m" << "МДНФ №" << count << "\033[0m" << std::endl;
            std::cout << "\033[1;33m" << "Изначальная таблица: " << "\033[0m" << "\033[1;34m" << table <<"\033[0m" << std::endl; 
            ++count;
    #endif
            stream->writeData(dnf);
        }
    }

	return 0;
}
