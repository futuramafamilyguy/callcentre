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

    char *name = argv[2];
    char buffer[100];
    int msg_len;

    printf("send msg to start chatting\n");
    while(1) {
        printf("%s: ", name);
        fgets(buffer, sizeof(buffer), stdin);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        if (strcmp(buffer, "exit")) {
            break;
        }

        msg_len = strlen(buffer);
        send(fd, buffer, msg_len, 0);
    }

    close(fd);

    free(res);

    return 0;
}
