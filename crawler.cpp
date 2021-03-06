// @author: Diga Widyaprana
// @matric: A0114171W

#include <chrono>
#include <cstdio>
#include <regex>
#include <thread>

#include "crawler.h"

namespace HTML = htmlcxx::HTML;

typedef struct Url Url;

Crawler::Crawler(Storage& store) : storage(store) {}

void Crawler::run() {
    while (true) {
        Url url = storage.get_next_url();

        // Terminate if we ran out of URLs to visit
        if (url.full().empty()) return;

        int socket_desc;
        struct addrinfo* addrinfo_res;

        // Resolve the URL to its IP Address
        int addrinfo_status =
            getaddrinfo(url.base.c_str(), "http", NULL, &addrinfo_res);
        if (addrinfo_status != 0) {
            // Error when getting address info.
            perror(gai_strerror(addrinfo_status));
            continue;
        }

        // Attempt to connect to the server
        bool connect_success = connect(addrinfo_res, &socket_desc);
        if (!connect_success) {
            perror("Cannot connect to any of the results");
            continue;
        }
        freeaddrinfo(addrinfo_res);

        // Construct the request header and send it over
        std::string request = construct_req_header(url.base, url.path);
        send(socket_desc, request.c_str(), request.size(), 0);

        // Measure the response time
        auto start = std::chrono::steady_clock::now();
        // Check if response has arrived, but don't retrieve it first
        recv(socket_desc, NULL, 0, MSG_PEEK);
        auto end = std::chrono::steady_clock::now();

        // Process the response
        char buffer[1024];
        int bytes_read;
        std::stringstream ss;
        while ((bytes_read = recv(socket_desc, buffer, sizeof buffer, 0)) > 0) {
            ss.write(buffer, bytes_read);
        }

        // In case the response terminated early
        if (bytes_read == -1) {
            perror("Error when receiving response.");
            continue;
        }

        // Parse the response into header and body so that the body can be
        // passed to the HTML parser
        HttpResponse response = parse_response(ss);

        HTML::ParserDom parser;
        tree<HTML::Node> dom = parser.parseTree(response.body);
        std::vector<std::string> links = extract_a_tag(dom);

        // Add all found URLs from the <a> tag into the storage.
        for (auto link = links.begin(); link != links.end(); ++link) {
            storage.add_url(parse_url_string(*link, url.base));
        }

        // Report the response time of the recently visited URL.
        std::chrono::microseconds duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double elapsed = duration.count()/1000.0;
        storage.report_res_time(url.base, elapsed);

        close(socket_desc);
        // Rate limiter, randomize between 1 to 10 seconds
        std::this_thread::sleep_for(
            std::chrono::seconds(1 + (std::rand() % 10)));
    } // end of while
}

bool Crawler::connect(struct addrinfo* ai_results, int* socket_desc) {
    // Result pointer
    struct addrinfo* rp;

    for (rp = ai_results; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in* addr;
        addr = (struct sockaddr_in *)rp->ai_addr;

        *socket_desc =
            socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (*socket_desc == -1) {
            perror("Error when opening a socket.");
            continue;
        }

        int connect_status =
            ::connect(*socket_desc, rp->ai_addr, rp->ai_addrlen);
        if (connect_status < 0) {
            close(*socket_desc);
            perror("Error when trying to connect.");
            continue;
        }
        break; // connect successful
    }

    if (rp == NULL) {
        // Ran out of ai_results, means failed to connect.
        return false;
    }
    return true;
}

std::string Crawler::construct_req_header(
        const std::string& host, const std::string& path) {
    std::ostringstream oss;
    oss << "GET " << path << " HTTP/1.0\r\n";
    oss << "Host: " << host << "\r\n";
    oss << "Accept: */*\r\n";
    oss << "User-Agent: digaw@u.nus.edu\r\n\r\n";
    return oss.str();
}

Crawler::HttpResponse Crawler::parse_response(std::stringstream& ss) {
    HttpResponse res;
    std::string header;
    std::string body;

    for (std::string line; std::getline(ss, line); ) {
        if (line.size() < 2) {
            // The blank line between header and body. Means we have reached the
            // end of the response header.
            break;
        }
        // Get line ignores the newline character.
        header += line + "\n";
    }
    res.header = header;

    for (std::string line; std::getline(ss, line); ) {
        if (line.size() < 2) {
            // Ignore the first blank line which is the boundary between header
            // and body.
            continue;
        }
        // Get line ignores the newline character.
        body += line + "\n";
    }
    res.body = body;
    return res;
}

std::vector<std::string> Crawler::extract_a_tag(tree<HTML::Node>& dom) {
    std::vector<std::string> links;
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    for (; it != end; ++it) {
        if (it->tagName() == "a" || it->tagName() == "A") {
            it->parseAttributes();
            links.push_back(it->attribute("href").second);
        }
    }
    return links;
}

Url Crawler::parse_url_string(
        std::string urlstr, const std::string& current_base_url) {
    Url new_url;
    // Matches "//www.example.com"
    std::regex regexp("//.+?[^/:](?=[?/]|$)");
    std::smatch match;
    std::regex_search(urlstr, match, regexp);

    if (match.empty()) {
        // If no match means it's a relative URL
        if (urlstr[0] != '/') {
            // insert '/' if not already there
            urlstr.insert(0, "/");
        }
        new_url.base = current_base_url;
        new_url.path = urlstr;
    } else {
        // Full URL
        // First 2 characters are "//", need to be removed
        new_url.base = match.str().substr(2);

        // Extract path from URL
        int offset = urlstr.find(new_url.base) + new_url.base.size();
        new_url.path = urlstr.substr(offset);

        if (new_url.path.empty()) {
            new_url.path = "/";
        }
    }
    return new_url;
}
