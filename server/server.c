#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main() {

    const char *port = "3000";

    struct addrinfo hints, *res, *p;
    int status, fd;
    char ipstring[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &res);

    struct sockaddr *sa = res->ai_addr;

    if (res->ai_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *) sa;
        inet_ntop(AF_INET, &(sin->sin_addr), ipstring, INET_ADDRSTRLEN);
    } else if (res->ai_family == AF_INET6) {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
        inet_ntop(AF_INET6, &(sin6->sin6_addr), ipstring, INET6_ADDRSTRLEN);
    }

    if (status == 0) {
        printf("my ip: %s\n", ipstring);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            // error handling
            continue;
        }

        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
            // error handling
            continue;
        }

        break;
    }

    if (listen(fd, 10) == 0) {
        struct sockaddr *addr = p->ai_addr;
        unsigned short port;
        
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)addr;
            port = ntohs(ipv4_addr->sin_port);
        } else if (p->ai_family == AF_INET6) {
            struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *)addr;
            port = ipv6_addr->sin6_port;
        }
        printf("listening on %d...\n", port);
    } else {
        //error handling
    }

    int inc_fd;
    struct sockaddr_storage inc_addr;
    socklen_t addr_size = sizeof(struct sockaddr_storage);
    inc_fd = accept(fd, (struct sockaddr *)&inc_addr, &addr_size);

    char inc_msg[100];
    recv(inc_fd, inc_msg, 100, 0);
    printf("received msg: %s\n", inc_msg);

    close(fd);
    close(inc_fd);

    free(res);

    return 0;
}
