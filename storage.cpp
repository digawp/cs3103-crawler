// @author: Diga Widyaprana
// @matric: A0114171W

#include <vector>

#include "storage.h"

typedef struct Url Url;

Storage::Storage(std::vector<std::string>& urls) {
    std::lock_guard<std::mutex> lock1(q_lock);
    std::lock_guard<std::mutex> lock2(blacklist_lock);
    for (auto url = urls.begin(); url != urls.end(); ++url) {
        url_blacklist.insert(*url);
        Url url_struct = { .base = *url, .path = "/" };
        to_visit_q.push_back(url_struct);
    }
}

void Storage::report_res_time(const std::string& base_url, const double time) {
    std::lock_guard<std::mutex> lock(url_log_lock);
    UrlLogEntry entry = url_log[base_url];
    entry.first += time;
    entry.second++;
}

void Storage::add_urls(std::vector<Url>& urls){
    std::lock_guard<std::mutex> lock1(q_lock);
    std::lock_guard<std::mutex> lock2(blacklist_lock);
    for (auto url = urls.begin(); url != urls.end(); ++url) {
        if (url_blacklist.find(url->full()) == url_blacklist.end()) {
            url_blacklist.insert(url->full());
            to_visit_q.push_back(*url);
        }
    }
}

Url Storage::get_next_url() {
    std::lock_guard<std::mutex> lock(q_lock);
    Url next = to_visit_q.front();
    to_visit_q.pop_front();
    return next;
}
