#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv) {
    
    if (argc < 3) {
        printf("missing argument\n");
        exit(-1);
    }

    printf("connecting to %s\n", argv[1]);

    struct addrinfo hints, *res, *p;
    int status, fd;
    char ipstring[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(argv[1], "3000", &hints, &res);

    struct sockaddr *sa = res->ai_addr;
    
    for (p = res; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) {
            // error handling

            perror("socket");  // Print the error message to stderr
            continue;
        }

        break;
    }

    if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
        // error handling
    }

    char *msg = argv[2];
    int msg_len, bytes_sent;
    msg_len = strlen(msg);

    bytes_sent = send(fd, msg, msg_len, 0);
    printf("%d out of %d byts sent\n", bytes_sent, msg_len);

    close(fd);

    free(res);

    return 0;
}
