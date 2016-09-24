// @author: Diga Widyaprana
// @matric: A0114171W

#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>

#include "crawler.h"
#include "storage.h"

Storage store;

void sigint_handler(int signum) {
    std::cout << "\nSigint " << signum << " received." << std::endl;
    std::cout << "Dump log to output/url_log.txt." << std::endl;
    store.dump_log();
    exit(signum);
}

void create_and_run_crawler(Storage& store) {
    Crawler crawler(store);
    crawler.run();
}

int main(int argc, char const *argv[]) {
    // register interrupt handler (eg ctrl-c)
    std::signal(SIGINT, sigint_handler);
    std::string seed = "seed";
    if (argc > 1) {
        seed = argv[1];
    }

    std::vector<std::string> urls;
    std::ifstream seed_file(seed);

    for (std::string line; std::getline(seed_file, line); ) {
        urls.push_back(line);
    }

    store.add_urls(urls);

    // Determine the number of threads to spawn, between 4-8
    int n = std::thread::hardware_concurrency();
    std::cout << n << std::endl;
    n = std::min(std::max(n, 4), 8);

    std::thread t(create_and_run_crawler, std::ref(store));

    for (int i = 0; i < n; ++i) {
        try {
            std::thread td(create_and_run_crawler, std::ref(store));
            td.detach();
        } catch(std::system_error& e) {
            perror("thread");
        }
    }

    // TODO: think of a termination condition! :/
    t.join();
    return 0;
}
