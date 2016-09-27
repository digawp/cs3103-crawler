// @author: Diga Widyaprana
// @matric: A0114171W

#ifndef CRAWLER_H_
#define CRAWLER_H_

// Includes for sockets
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>
#include <vector>

#include "storage.h"
#include "structs.h"

// HTML parser
#include <htmlcxx/html/ParserDom.h>

class Crawler {
private:
    /**
     * A struct representing a HTTP response.
     * One can obtain the header and the body of the HTTP response.
     */
    typedef struct {
        std::string header;
        std::string body;
    } HttpResponse;

    /**
     * A reference to the shared storage instance.
     */
    Storage& storage;

public:
    Crawler(Storage& store);

    /**
     * @brief      Runs the crawler.
     */
    void run();

private:
    /**
     * @brief      Connects the specified socket descriptor to the first
     *             addressinfo result that can be connected to.
     *
     * @param      ai_results   The addressinfo result.
     * @param      socket_desc  The socket file descriptor.
     *
     * @return     - true if connection to any one of the addressinfo result is
     *             successful
     *             - false if cannot connect to any of the addressinfo results
     */
    bool connect(struct addrinfo* ai_results, int* socket_desc);

    /**
     * @brief      Constructs a HTTP request header to the specified host and
     *             the specified path
     *
     * @param[in]  host  The host
     * @param[in]  path  The path
     *
     * @return     The HTTP request header
     */
    std::string construct_req_header(const std::string& host, const std::string& path);

    /**
     * @brief      Parse a stringstream of HTTP response into a
     *             @code{HttpResponse} struct
     *
     * @param      ss    the stringstream containing HTTP response
     *
     * @return     A @code{HttpResponse} struct of the HTTP response string.
     */
    HttpResponse parse_response(std::stringstream& ss);

    /**
     * @brief      Extracts the <a> tags of the DOM tree supplied to the method
     *
     * @param      dom   The DOM tree
     *
     * @return     A vector of URLs from the <a> tags in the DOM tree.
     */
    std::vector<std::string> extract_a_tag(tree<htmlcxx::HTML::Node>& dom);

    /**
     * @brief      Parse a URL string into a @code {Url} struct
     *
     * @param[in]  urlstr            The URL string
     * @param[in]  current_base_url  The base URL the crawler is crawling when
     *             obtaining the URL string
     *
     * @return     The @code{Url} struct of the URL string
     */
    struct Url parse_url_string(std::string urlstr, const std::string& current_base_url);
};
#endif // CRAWLER_H_
