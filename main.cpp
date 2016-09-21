// @author: Diga Widyaprana
// @matric: A0114171W
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

std::string construct_req_header(std::string host, std::string path) {
    std::ostringstream oss;
    oss << "GET " << path << " HTTP/1.1\r\n";
    oss << "Host: " << host << "\r\n";
    oss << "Accept: */*" << "\r\n";
    oss << "Connection: close" << "\r\n" << "\r\n";
    return oss.str();
}

int main(int argc, char const *argv[])
{
    int socket_desc;
    struct addrinfo* addrinfo_res;
    const char* url = "www.comp.nus.edu.sg";

    if (argc > 1)
    {
        url = argv[1];
    }

    int addrinfo_status = getaddrinfo(url, "http", NULL, &addrinfo_res);

    if (addrinfo_status != 0)
    {
        perror(gai_strerror(addrinfo_status));
        return 1;
    }

    bool connect_success = connect(addrinfo_res, &socket_desc);

    if (!connect_success)
    {
        perror("Cannot connect to any of the results");
        return 1;
    }
    freeaddrinfo(addrinfo_res);

    std::string request = construct_req_header(url, "/");

    std::cout << "Request:" << std::endl;
    std::cout << request;
    std::cout << "============================" << std::endl << std::endl;

    send(socket_desc, request.c_str(), request.size(), 0);

    char buffer[1024];
    int bytes_read;

    std::stringstream ss;
    while ((bytes_read = recv(socket_desc, buffer, sizeof buffer, 0)) > 0) {
        ss.write(buffer, bytes_read);
    }
    std::cout << "Response: " << std::endl;
    std::string line;

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

    // std::cout << ss.str() << std::endl;

    // In case the response terminated early
    if (bytes_read == -1)
    {
        perror("recv");
        return 1;
    }

    close(socket_desc);
    return 0;
}
