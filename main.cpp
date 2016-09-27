// @author: Diga Widyaprana
// @matric: A0114171W

#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "crawler.h"
#include "storage.h"

Storage store;

/**
 * @brief      Handles interrupt signal. Dump the URLs visited and exit the
 *             program.
 *
 * @param[in]  signum  The signal number received.
 */
void sigint_handler(int signum) {
    std::cout << "\nSigint " << signum << " received." << std::endl;
    std::cout << "Dump URLs to url_log.txt." << std::endl;
    store.dump_log();
    exit(signum);
}

/**
 * @brief      The function to be called for each thread.
 *             Creates a crawler and run it.
 *
 * @param      store  The shared storage to be used by the crawlers.
 */
void create_and_run_crawler(Storage& store) {
    Crawler crawler(store);
    crawler.run();
}

int main(int argc, char const *argv[]) {
    // Register interrupt handler (eg ctrl-c)
    std::signal(SIGINT, sigint_handler);

    std::cout << "Crawling with seed URLs from " << seed << std::endl;
    std::cout << "Press Ctrl+c to terminate." << std::endl;

    // Read the seed file to be used as the seed URLs for the crawlers.
    std::string seed = "seed";
    if (argc > 1) {
        seed = argv[1];
    }
    std::ifstream seed_file(seed);
    std::vector<std::string> urls;
    for (std::string line; std::getline(seed_file, line); ) {
        urls.push_back(line);
    }

    // Add the seed URLs to the shared storage.
    store.add_urls(urls);

    // Determine the number of threads (crawlers) to spawn, between 4-8
    int n = std::thread::hardware_concurrency();
    std::cout << "Max hardware concurrency: " << n << std::endl;
    n = std::min(std::max(n, 4), 8);
    std::cout << "Threads to be used: " << n << std::endl;

    std::vector<std::shared_ptr<std::thread>> threads;

    for (int i = 0; i < n; ++i) {
        try {
            auto td = std::make_shared<std::thread>(create_and_run_crawler,
                                                    std::ref(store));
            threads.push_back(td);
        } catch(std::system_error& e) {
            perror("thread");
        }
    }

    // TODO: think of a termination condition! :/
    for (auto t_ptr = threads.begin(); t_ptr != threads.end(); ++t_ptr) {
        (*t_ptr)->join();
    }

    std::cout << "Ran out of URLs to visit. Dump URLs to url_log.txt.\n";
    store.dump_log();
    return 0;
}
