// @author: Diga Widyaprana
// @matric: A0114171W

#ifndef STORAGE_H_
#define STORAGE_H_

#include <deque>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "structs.h"

/**
 * @brief      Class for storage of URLs visited, domain and response times, etc
 */
class Storage {
private:
    typedef struct {
        double time = 0;
        int queued = 0;
        int visited = 0;
    } UrlLogVals;
    typedef std::unordered_map<std::string, UrlLogVals> UrlLog;
    typedef std::unordered_set<std::string> BlackList;

    // Locks to prevent race conditions
    std::mutex url_log_lock;
    std::mutex blacklist_lock;
    std::mutex q_lock;

    // The limit to the number of domains in the log.
    unsigned int log_limit = 30;

    // Count of number of domains visited.
    unsigned int visited_count = 0;

    /**
     * The list of base URLs (domains) visited and its response time, and other auxiliary data.
     */
    UrlLog url_log;

    /**
     * The list of URLs visited, to prevent a URL from being visited more than once.
     */
    BlackList url_blacklist;

    /**
     * The queue of URLs to be visited next.
     */
    std::deque<struct Url> to_visit_q;

public:
    void set_url_limit(const unsigned int limit);

    unsigned int get_url_limit();

    /**
     * @brief      API for crawlers to report response time of a domain.
     *             The unit of time is milliseconds.
     *
     * @param[in]  url   The base url of the domain
     * @param[in]  time  The response time
     */
    void report_res_time(const std::string& url, const double time);

    /**
     * @brief      Adds a URL to the queue of URLs to be visited if it is not
     *             visited yet.
     *
     * @param[in]  url  The url
     */
    void add_url(Url url);

    /**
     * @brief      Adds string urls to the queue of URLs to be visited if it is
     *             not visited yet.
     *
     * @param      urlstrs  The url string. It should be a base URL without the
     *                      protocol (HTTP assumed).
     */
    void add_urls(std::vector<std::string>& urlstrs);

    /**
     * @brief      Gets the next URL to be visited.
     *
     * @return     The URL struct.
     */
    struct Url get_next_url();

    /**
     * @brief      Dumps the log of URLs that have been visited and its average
     *             response time in ms.
     */
    void dump_log();
};
#endif // STORAGE_H_
