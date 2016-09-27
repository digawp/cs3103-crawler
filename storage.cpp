// @author: Diga Widyaprana
// @matric: A0114171W

#include <fstream>
#include <vector>

#include "storage.h"

typedef struct Url Url;

void Storage::report_res_time(const std::string& base_url, const double time) {
    std::lock_guard<std::mutex> lock(url_log_lock);
    url_log[base_url].time += time;
    url_log[base_url].visited++;
}

void Storage::add_url(Url url){
    std::lock_guard<std::mutex> lock1(q_lock);
    std::lock_guard<std::mutex> lock2(blacklist_lock);
    if (url_blacklist.find(url.full()) == url_blacklist.end() &&
            url_log[url.base].queued < 20) {
        // Each base url will only be visited 20 times (TODO: may need to be
        // adjusted)
        url_log[url.base].queued++;
        url_blacklist.insert(url.full());
        to_visit_q.push_back(url);
    }
}

void Storage::add_urls(std::vector<Url>& urls){
    for (auto url = urls.begin(); url != urls.end(); ++url) {
        add_url(*url);
    }
}

void Storage::add_urls(std::vector<std::string>& urlstrs) {
    for (auto url = urlstrs.begin(); url != urlstrs.end(); ++url) {
        Url url_struct = { .base = *url, .path = "/" };
        add_url(url_struct);
    }
}

Url Storage::get_next_url() {
    std::lock_guard<std::mutex> lock(q_lock);
    // TODO: handle case when we ran out of URL
    Url next;
    if (to_visit_q.empty()) {
        return next;
    }
    next = to_visit_q.front();
    to_visit_q.pop_front();
    return next;
}

void Storage::dump_log() {
    std::lock_guard<std::mutex> lock(url_log_lock);
    std::ofstream fs;
    fs.open("url_log.txt");
    for (auto log_entry = url_log.begin(); log_entry != url_log.end();
            ++log_entry) {
        UrlLogVals log_val = log_entry->second;
        if (log_val.visited == 0) {
            continue;
        }
        double avg_time = log_val.time / static_cast<double>(log_val.visited);
        fs << log_entry->first << "\t\t" << avg_time << " ms" << std::endl;
    }
    fs.close();
}
