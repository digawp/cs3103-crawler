// @author: Diga Widyaprana
// @matric: A0114171W

#include <fstream>
#include <vector>

#include "storage.h"

typedef struct Url Url;

Storage::Storage(std::vector<std::string>& urlstrs) {
    std::lock_guard<std::mutex> lock1(q_lock);
    std::lock_guard<std::mutex> lock2(blacklist_lock);
    for (auto url = urlstrs.begin(); url != urlstrs.end(); ++url) {
        url_blacklist.insert(*url);
        Url url_struct = { .base = *url, .path = "/" };
        to_visit_q.push_back(url_struct);
    }
}

void Storage::report_res_time(const std::string& base_url, const double time) {
    std::lock_guard<std::mutex> lock(url_log_lock);
    // If url_log[base_url] does not exist, one will be created
    url_log[base_url].first += time;
    url_log[base_url].second++;
}

void Storage::add_url(Url url){
    std::lock_guard<std::mutex> lock1(q_lock);
    std::lock_guard<std::mutex> lock2(blacklist_lock);
    if (url_blacklist.find(url.full()) == url_blacklist.end()) {
        url_blacklist.insert(url.full());
        to_visit_q.push_back(url);
    }
}

void Storage::add_urls(std::vector<Url>& urls){
    for (auto url = urls.begin(); url != urls.end(); ++url) {
        add_url(*url);
    }
}

Url Storage::get_next_url() {
    std::lock_guard<std::mutex> lock(q_lock);
    Url next = to_visit_q.front();
    to_visit_q.pop_front();
    return next;
}

void Storage::dump_log() {
    std::ofstream fs;
    fs.open("output/url_log.txt");
    for (auto log_entry = url_log.begin(); log_entry != url_log.end();
            ++log_entry) {
        double avg_time = log_entry->second.first / log_entry->second.second;
        fs << log_entry->first << "\t\t" << avg_time << " ms" << std::endl;
    }
    fs.close();
}
