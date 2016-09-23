// @author: Diga Widyaprana
// @matric: A0114171W

#include <fstream>

#include "crawler.h"
#include "storage.h"

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
    Crawler crawler(store);
    crawler.run();
    store.dump_log();
    return 0;
}
