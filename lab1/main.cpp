#include <iostream>
#include <string> 
#include <set>
#include <memory>
#include <thread>
#include <deque>
#include <mutex> 

#include "impl.h"
#include "dnf.h"
#include "streamData.h"
#include "machine.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<streamData> stream;
    stream = streamData::create<streamDataConsole>(std::cin, std::cout);

    Mealy mealy(std::string("../f_table.txt"), std::string("../g_table.txt"));
    std::map<std::string, std::unordered_set<std::string>> minimize;
    mealy.coddingStates();
    mealy.generateDTriggerSndf(minimize);
    mealy.generateOutputSndf(minimize);

    for (auto &pair : minimize) {
        std::cout << pair.first << " ";
        for (auto &it : pair.second) {
            std::cout << it << " ";
        }
        std::cout << std::endl;
    }

    auto deq = mealy.format(minimize);

    auto it = minimize.begin();
    while (!deq.empty()) {
        std::string sndf = deq.front();
        std::cout << it->first << std::endl;
        DNF dnf(sndf);
        dnf.Minimize();
        stream->writeData(dnf);
        deq.pop_front();
        it = std::next(it);
    }

	return 0;
}
