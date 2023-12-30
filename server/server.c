#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

char* concat(const char *s1, const char *s2);

int main() {
    
    char * responses[] = {
        "go google it",
        "booking office for gondolas and punting is just on the right",
        "move to melbourne and dont come back",
        "without darkness there is no light",
        "aot s2 is the best season change my mind",
        "check out my tumblr"
    };

    srand(time(NULL));

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

    char inc_msg[1024];
    char *out_msg;
    int out_msg_len;
    memset(&inc_msg, 0, sizeof inc_msg);

    while (1) {
        inc_fd = accept(fd, (struct sockaddr *)&inc_addr, &addr_size);
        printf("new connection recvd\n");

        if (!fork()) {
            close(fd);
            while (recv(inc_fd, inc_msg, 1024, 0)) {
                memset(&out_msg, 0, sizeof out_msg);
                char *resp = responses[rand() % sizeof(responses)/sizeof(responses[0])];
                out_msg = concat(inc_msg, resp);
                out_msg_len = strlen(out_msg);
                send(inc_fd, out_msg, out_msg_len, 0);
                free(out_msg);
                memset(&inc_msg, 0, sizeof inc_msg);
            }

            close(inc_fd);
            exit(0);
        }
    }
    
    printf("received msg: %s\n", inc_msg);

    close(fd);
    free(res);

    return 0;
}

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 3); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, "? ");
    strcat(result, s2);
    return result;
}
