// @author: Diga Widyaprana
// @matric: A0114171W

#include "crawler.h"

int main(int argc, char const *argv[])
{
    std::string url = "www.comp.nus.edu.sg";

    if (argc > 1)
    {
        url = argv[1];
    }

    Crawler crawler;
    crawler.run(url);
    return 0;
}
