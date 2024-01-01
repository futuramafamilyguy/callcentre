#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include "ccp/ccp.h"

#define HOST "localhost"
#define PORT "3000"

#define FD_COUNT 2
#define FD_STDIN 0

#define BUF_SIZE 256

int get_server_socket(char *host, char *port); 

int get_server_socket(char *host, char *port)
{
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

    if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
        perror("connect");
        close(sock);
        exit(1);
    }

    free(res);

    return sock;
}

int main(int argc, char** argv)
{ 
    if (argc < 2) {
        printf("missing argument\n");
        exit(-1);
    }

    int serverfd;
    char *username;
    char buf[BUF_SIZE];
    struct pollfd *pfds = malloc(sizeof *pfds * FD_COUNT);

    serverfd = get_server_socket(HOST, PORT);
    username = argv[1];

    pfds[0].fd = FD_STDIN;
    pfds[0].events = POLLIN;
    pfds[1].fd = serverfd;
    pfds[1].events = POLLIN;

    while (1) {
        int poll_count = poll(pfds, FD_COUNT, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for(int i = 0; i < FD_COUNT; i++) {
            if (poll_count == 0) {
                break;
            }

            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == FD_STDIN) { // if data is from stdin then we send data
                    int msg_len;

                    memset(&buf, 0, sizeof buf);
                    fgets(buf, sizeof(buf), stdin);
                    if (buf[strlen(buf) - 1] == '\n') {
                        buf[strlen(buf) - 1] = '\0';
                    }

                    if (!strcmp(buf, "exit")) {
                        close(serverfd);
                        exit(0);
                    }

                    printf("%s: %s\n", argv[1], buf);
                    msg_len = strlen(buf);
                    send(serverfd, buf, msg_len, 0);
                } else { // if data is from server socket then we display
                    memset(&buf, 0, sizeof buf);
                    if (recv(serverfd, buf, BUF_SIZE, 0) == 0) {
                        printf("server hung up gg\n");
                        close(serverfd);
                        exit(0);
                    }

                    char *server_chat_name;
                    char *msg;
                    parse_ccp(buf, server_chat_name, msg);
                    
                    printf("%s: %s\n", server_chat_name, msg);
                }
                
                poll_count--;
            }
        }
    }

    return 0;
}
