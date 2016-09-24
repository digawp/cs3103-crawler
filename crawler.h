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
    typedef struct {
        std::string header;
        std::string body;
    } HttpResponse;

    Storage& storage;

public:
    Crawler(Storage& store);
    void run();

private:
    bool connect(struct addrinfo* ai_results, int* socket_desc);

    std::string construct_req_header(const std::string& host, const std::string& path);

    HttpResponse parse_response(std::stringstream& ss);

    std::vector<std::string> extract_a_tag(tree<htmlcxx::HTML::Node>& dom);

    struct Url parse_url_string(std::string urlstr, const std::string& current_base_url);
};
#endif // CRAWLER_H_
