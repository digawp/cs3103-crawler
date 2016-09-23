// @author: Diga Widyaprana
// @matric: A0114171W

#include <fstream>
#include <iostream>
#include <thread>

#include "crawler.h"
#include "storage.h"

void create_and_run_crawler(Storage& store) {
    Crawler crawler(store);
    crawler.run();
}

int main(int argc, char const *argv[])
{
    std::string seed = "seed";
    if (argc > 1) {
        seed = argv[1];
    }

    std::vector<std::string> urls;
    std::ifstream seed_file(seed);

    for (std::string line; std::getline(seed_file, line); ) {
        urls.push_back(line);
    }

    Storage store(urls);

    int n = std::thread::hardware_concurrency();
    std::cout << n << std::endl;
    n = n < 4 ? 4 : n;

    for (int i = 0; i < n; ++i) {
        try {
            std::thread t(create_and_run_crawler, std::ref(store));
            t.detach();
        } catch(std::system_error& e) {
            perror("thread");
        }
    }

    // TODO: think of a termination condition! :/
    int lel;
    std::cin >> lel;
    store.dump_log();
    return 0;
}
