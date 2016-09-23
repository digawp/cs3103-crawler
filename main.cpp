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
#include <sstream>

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

void print_response_header_DEBUG(std::stringstream& ss) {
    std::string line;
    std::cout << "Response: " << std::endl;

    // Print response header
    while(true) {
        std::getline(ss, line);
        if (line.size() < 2)
        {
            std::cout << line.size() << std::endl;
            std::cout << line << std::endl;
            break;
        }
        std::cout << line << std::endl;
    }
}

void dump_html_DEBUG(std::stringstream& ss) {
    std::ofstream myfile;
    myfile.open ("output/dump.html");
    for (std::string line; std::getline(ss, line); ) {
        if (line.size() < 2) {
            continue;
        }
        myfile << line << std::endl;
    }
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

int main(int argc, char const *argv[])
{
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

    print_response_header_DEBUG(ss);

    // In case the response terminated early
    if (bytes_read == -1) {
        perror("recv");
        return 1;
    }

    dump_response_DEBUG(ss);
    dump_html_DEBUG(ss);

    // TODO: extract out the header, just pass the response body to parser
    std::string html = ss.str();

    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(html);

    std::ofstream dom_file;
    dom_file.open("output/dom.html");
    dom_file << dom << std::endl;
    dom_file.close();

    dump_links_DEBUG(dom);

    close(socket_desc);
    return 0;
}
