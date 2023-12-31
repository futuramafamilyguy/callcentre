#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define PORT "3000"

#define MAX_CONN 5

#define FD_STDIN 0

#define BUF_SIZE 256

int get_listener_socket(char *port)
{
    int sock, rv;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock == -1) {
            continue;
        }

        if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            continue;
        }

        break;
    }

    if (listen(sock, MAX_CONN) == -1) {
        return -1;
    }

    free(res);

    return sock;
}

int add_pfd(struct pollfd pfds[], int newfd, int *fd_count, int fd_size)
{
    if (*fd_count == fd_size) {
        return 1;
    }

    pfds[*fd_count].fd = newfd;
    pfds[*fd_count].events = POLLIN;
    (*fd_count)++;

    return 0;
}

void rm_pfd(struct pollfd pfds[], int i, int *fd_count)
{
    pfds[i] = pfds[*fd_count - 1];
    (*fd_count)--;
}

int main() {

    int listenerfd;
    int fd_count = 0;
    int fd_size = MAX_CONN + 2; // 5 inc connections + listener and stdin
    struct pollfd *pfds = malloc(sizeof(struct pollfd) * fd_size);

    char buf[BUF_SIZE];

    listenerfd = get_listener_socket(PORT);
    add_pfd(pfds, listenerfd, &fd_count, fd_size); // add listener
    add_pfd(pfds, FD_STDIN, &fd_count, fd_size); // add stdin

    while(1) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for(int i = 0; i < fd_count; i++) {
            if (poll_count == 0) {
                break;
            }

            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == listenerfd) { // if data is from listener then add new fd to be polled
                    printf("connection revd\n");
                    struct sockaddr_storage inc_addr;
                    socklen_t addr_size = sizeof(struct sockaddr_storage);

                    int incfd = accept(listenerfd, (struct sockaddr *)&inc_addr, &addr_size);
                    if (add_pfd(pfds, incfd, &fd_count, fd_size) == 1) {
                        printf("connections full so rejecting\n");
                        close(incfd);
                    }
                } else { // if data is from client socket then display
                    int senderfd = pfds[i].fd;
                    int bytes = recv(senderfd, buf, BUF_SIZE, 0);

                    if (bytes <= 0) {
                        printf("something happened goodbye sender %d\n", senderfd);
                        rm_pfd(pfds, i, &fd_count);
                        close(senderfd);
                    } else {
                        printf("sender %d: %s\n", senderfd, buf);
                    }
                }
            }
        }
    }

    close(listenerfd);
    free(pfds);

    return 0;
}
