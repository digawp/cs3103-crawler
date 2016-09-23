// @author: Diga Widyaprana
// @matric: A0114171W

// Includes for sockets
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <vector>

// HTML parser
#include <htmlcxx/html/ParserDom.h>

namespace HTML = htmlcxx::HTML;

// =================================
// BEGIN For debugging purposes
// =================================
void print_request_DEBUG(const std::string content) {
    std::cout << "Request" << std::endl;
    std::cout << content;
    std::cout << "============================" << std::endl << std::endl;
}

void dump_html_DEBUG(const std::string& ss) {
    std::ofstream myfile;
    myfile.open ("output/dump.html");
    myfile << ss;
    myfile.close();
}

void dump_links_DEBUG(tree<HTML::Node>& dom) {
    //Dump all links in the tree
    std::ofstream flinks;
    flinks.open("output/links.txt");
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    for (; it != end; ++it)
    {
        if (it->tagName() == "a" || it->tagName() == "A")
        {
            it->parseAttributes();
            flinks << (it->attribute("href").second) << std::endl;
        }
    }
    flinks.close();
}

void dump_response_DEBUG(std::stringstream& ss) {
    std::ofstream file;
    file.open("output/responsedump.txt");
    file << ss.str() << std::endl;
    file.close();
}
// =================================
// END For debugging purposes
// =================================


bool connect(struct addrinfo* ai_results, int* socket_desc) {
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

std::string construct_req_header(const std::string host, const std::string path) {
    std::ostringstream oss;
    oss << "GET " << path << " HTTP/1.0\r\n";
    oss << "Host: " << host << "\r\n";
    oss << "Accept: */*" << "\r\n" << "\r\n";
    // oss << "Connection: close" << "\r\n" << "\r\n";
    return oss.str();
}

typedef struct {
    std::string header;
    std::string body;
} HttpResponse;

typedef struct {
    std::string base;
    std::string path;
    std::string full() { return base + path; };
} Url;

HttpResponse parse_response(std::stringstream& ss) {
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

std::vector<std::string> extract_a_tag(tree<HTML::Node>& dom) {
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

int main(int argc, char const *argv[]) {
    int socket_desc;
    struct addrinfo* addrinfo_res;
    const char* url = "www.google.com.sg";

    if (argc > 1)
    {
        url = argv[1];
    }

    int addrinfo_status = getaddrinfo(url, "http", NULL, &addrinfo_res);
    if (addrinfo_status != 0) {
        perror(gai_strerror(addrinfo_status));
        return 1;
    }

    bool connect_success = connect(addrinfo_res, &socket_desc);
    if (!connect_success) {
        perror("Cannot connect to any of the results");
        return 1;
    }
    freeaddrinfo(addrinfo_res);

    std::string request = construct_req_header(url, "/?gfe_rd=cr&ei=y8LkV6P9MpDI8Ae83JNo");

    print_request_DEBUG(request);

    send(socket_desc, request.c_str(), request.size(), 0);

    char buffer[1024];
    int bytes_read;

    std::stringstream ss;
    while ((bytes_read = recv(socket_desc, buffer, sizeof buffer, 0)) > 0) {
        ss.write(buffer, bytes_read);
    }

    // In case the response terminated early
    if (bytes_read == -1) {
        perror("recv");
        return 1;
    }

    HttpResponse response = parse_response(ss);
    std::cout << response.header;

    dump_response_DEBUG(ss);
    dump_html_DEBUG(response.body);

    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(response.body);

    // std::ofstream dom_file;
    // dom_file.open("output/dom.html");
    // dom_file << dom << std::endl;
    // dom_file.close();

    dump_links_DEBUG(dom);

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

    close(socket_desc);
    return 0;
}
