#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define HOST "localhost"
#define PORT "3000"

int get_server_socket(char *host, char *port) {
    int sock, rv;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock == -1) {
            continue;
        }

        break;
    }

    free(res);

    return sock;
}

int main(int argc, char** argv) {
    
    if (argc < 2) {
        printf("missing argument\n");
        exit(-1);
    }

    int serverfd = get_server_socket();

    if (connect(serverfd, p->ai_addr, p->ai_addrlen) == -1) {
        perror("connect");
        close(serverfd);
        exit(EXIT_FAILURE);
    }

    char *name = argv[2];
    char buffer[1024];
    int msg_len;

    printf("send msg to start chatting\n");
    while(1) {
        memset(&buffer, 0, sizeof buffer);
        printf("%s: ", name);
        fgets(buffer, sizeof(buffer), stdin);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        if (!strcmp(buffer, "exit")) {
            break;
        }

        msg_len = strlen(buffer);
        send(serverfd, buffer, msg_len, 0);

        recv(serverfd, buffer, 1024, 0);
        printf("chatgpt: %s\n", buffer);
    }

    close(fd);

    free(res);

    return 0;
}
