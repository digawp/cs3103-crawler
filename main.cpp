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
    std::cout << "Dump URLs to url_log.txt." << std::endl;
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

    std::cout << "Crawling with seed URLs from " << seed << std::endl;
    std::cout << "Press Ctrl+c to terminate." << std::endl;

    std::vector<std::string> urls;
    std::ifstream seed_file(seed);

    for (std::string line; std::getline(seed_file, line); ) {
        urls.push_back(line);
    }

    store.add_urls(urls);

    // Determine the number of threads to spawn, between 4-8
    int n = std::thread::hardware_concurrency();
    std::cout << "Max hardware concurrency: " << n << std::endl;
    n = std::min(std::max(n, 4), 8);
    std::cout << "Threads to be used: " << n << std::endl;

    std::thread t(create_and_run_crawler, std::ref(store));

    // 1 is already created above
    for (int i = 1; i < n; ++i) {
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
