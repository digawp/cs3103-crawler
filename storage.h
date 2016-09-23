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
    typedef std::pair<double, int> UrlLogEntry;
    typedef std::unordered_map<std::string, UrlLogEntry> UrlLog;
    typedef std::unordered_set<std::string> BlackList;

    std::mutex url_log_lock;
    std::mutex blacklist_lock;
    std::mutex q_lock;

    UrlLog url_log;
    BlackList url_blacklist;
    std::deque<struct Url> to_visit_q;

public:
    Storage(std::vector<std::string>& urls);

    void report_res_time(const std::string& url, const double time);

    void add_urls(std::vector<struct Url>& urls);

    struct Url get_next_url();
};
#endif // STORAGE_H_
