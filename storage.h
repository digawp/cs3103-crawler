// @author: Diga Widyaprana
// @matric: A0114171W

#ifndef STORAGE_H_
#define STORAGE_H_

#include <deque>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "structs.h"

class Storage {
private:
    typedef struct {
        double time = 0;
        int queued = 0;
        int visited = 0;
    } UrlLogVals;
    typedef std::unordered_map<std::string, UrlLogVals> UrlLog;
    typedef std::unordered_set<std::string> BlackList;

    std::mutex url_log_lock;
    std::mutex blacklist_lock;
    std::mutex q_lock;

    UrlLog url_log;
    BlackList url_blacklist;
    std::deque<struct Url> to_visit_q;

public:
    void report_res_time(const std::string& url, const double time);

    void add_url(Url urls);
    void add_urls(std::vector<struct Url>& urls);
    void add_urls(std::vector<std::string>& urlstrs);

    struct Url get_next_url();

    void dump_log();
};
#endif // STORAGE_H_
