// @author: Diga Widyaprana
// @matric: A0114171W

#include <chrono>
#include <cstdio>
#include <regex>

#include "crawler.h"
#include "structs.h"

// TODO: Temporary, remove on release!
#include <iostream>

namespace HTML = htmlcxx::HTML;

typedef struct Url Url;

void Crawler::run(const std::string& starting_url) {
    std::string base_url = starting_url;
    std::string path = "/";
    while (true) {
        int socket_desc;
        struct addrinfo* addrinfo_res;

        int addrinfo_status = getaddrinfo(base_url.c_str(), "http", NULL, &addrinfo_res);
        if (addrinfo_status != 0) {
            perror(gai_strerror(addrinfo_status));
            return;
        }

        bool connect_success = connect(addrinfo_res, &socket_desc);
        if (!connect_success) {
            perror("Cannot connect to any of the results");
            return;
        }
        freeaddrinfo(addrinfo_res);

        std::string request = construct_req_header(base_url, path);
        send(socket_desc, request.c_str(), request.size(), 0);

        auto start = std::chrono::steady_clock::now();
        recv(socket_desc, NULL, 0, MSG_PEEK);
        auto end = std::chrono::steady_clock::now();

        char buffer[1024];
        int bytes_read;
        std::stringstream ss;
        while ((bytes_read = recv(socket_desc, buffer, sizeof buffer, 0)) > 0) {
            ss.write(buffer, bytes_read);
        }

        // In case the response terminated early
        if (bytes_read == -1) {
            perror("recv");
            return;
        }

        HttpResponse response = parse_response(ss);

        HTML::ParserDom parser;
        tree<HTML::Node> dom = parser.parseTree(response.body);
        std::vector<std::string> links = extract_a_tag(dom);

        for (auto link = links.begin(); link != links.end(); ++link) {
            Url url;
            std::regex regexp("//.+?[^/:](?=[?/]|$)");
            std::smatch match;
            std::regex_search(*link, match, regexp);
            if (match.empty()) {
                std::cout << "Relative url" << std::endl;
                continue;
            }
            url.base = match.str().substr(2);
            int offset = link->find(url.base) + url.base.size();
            url.path = link->substr(offset);
            if (url.path.empty()) {
                url.path = "/";
            }
            std::cout << url.base << "\t" << url.path << std::endl;
        }
        std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << elapsed.count()/1000.0 << std::endl;
        close(socket_desc);
        break;
    } // end of while
}

bool Crawler::connect(struct addrinfo* ai_results, int* socket_desc) {
    struct addrinfo* rp;

    for (rp = ai_results; rp != NULL; rp = rp->ai_next)
    {
        struct sockaddr_in* addr;
        addr = (struct sockaddr_in *)rp->ai_addr;

        *socket_desc =
            socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (*socket_desc == -1)
        {
            perror("client: socket");
            continue;
        }
        int connect_status =
            ::connect(*socket_desc, rp->ai_addr, rp->ai_addrlen);
        if (connect_status < 0)
        {
            close(*socket_desc);
            perror("client: connect");
            continue;
        }
        break; // connect successful or ran out of rp
    }

    if (rp == NULL)
    {
        return false;
    }
    return true;
}

std::string Crawler::construct_req_header(const std::string& host, const std::string& path) {
    std::ostringstream oss;
    oss << "GET " << path << " HTTP/1.0\r\n";
    oss << "Host: " << host << "\r\n";
    oss << "Accept: */*" << "\r\n" << "\r\n";
    // oss << "Connection: close" << "\r\n" << "\r\n";
    return oss.str();
}

Crawler::HttpResponse Crawler::parse_response(std::stringstream& ss) {
    HttpResponse res;
    std::string header;
    std::string body;

    for (std::string line; std::getline(ss, line); ) {
        if (line.size() < 2) {
            break;
        }
        header += line + "\n";
    }
    res.header = header;

    for (std::string line; std::getline(ss, line); ) {
        if (line.size() < 2) {
            continue;
        }
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
