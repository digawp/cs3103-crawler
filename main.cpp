// @author: Diga Widyaprana
// @matric: A0114171W

#include "crawler.h"
#include "storage.h"

int main(int argc, char const *argv[])
{
    std::string url = "www.comp.nus.edu.sg";

    if (argc > 1) {
        url = argv[1];
    }

    std::vector<std::string> urls;
    urls.push_back(url);

    Storage store(urls);

    Crawler crawler;
    crawler.run(url);
    return 0;
}
