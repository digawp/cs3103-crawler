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
std::string seed_file_name = "seed";

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
    exit(0);
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

void parse_options(int argc, char const *argv[]) {
    if (argc == 1) {
        return; // nothing to set
    }
    if (argc > 3) {
        std::cout << "Usage: <program-name> [<seed file name> | <url log limit>]" << std::endl;
        std::cout << "Integers will be assumed as url log limit." << std::endl;
        std::cout << "Other than that it will be assumed as seed file name." << std::endl;
        std::cout << "If multiple integers or non-integers is given, only the last ones are used." << std::endl;
    }
    for (int i = 1; i < argc; ++i) {
        if (atoi(argv[i])) {
            store.set_url_limit(atoi(argv[i]));
        } else {
            seed_file_name = argv[i];
        }
    }
}

int main(int argc, char const *argv[]) {
    // Register interrupt handler (eg ctrl-c)
    std::signal(SIGINT, sigint_handler);

    // This will set the @code {seed_file_name} and @code {store}'s log limit
    parse_options(argc, argv);

    std::cout << "Crawling with seed URLs from " << seed_file_name << std::endl;
    std::cout << "Will stop when " << store.get_url_limit() << " domains visited." << std::endl;
    std::cout << "Press Ctrl+c to terminate." << std::endl;

    // Read the seed file to be used as the seed URLs for the crawlers.
    std::ifstream seed_file(seed_file_name);
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
