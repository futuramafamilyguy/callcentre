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

int rm_pfd(struct pollfd pfds[], int i, int *fd_count)
{
    if (i >= *fd_count) {
        return 1;
    }

    pfds[i] = pfds[*fd_count - 1];
    (*fd_count)--;

    return 0;
}

int valid_client_fd(struct pollfd pfds[], int fd, int fd_size)
{
    for (int i = 2; i < sizeof(struct pollfd) * fd_size; i++) { // start from 2 to skip stdin and listener fd
        if (pfds[i].fd == fd) {
            return 1;
        }
    }

    return 0;
}

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 2); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("missing argument\n");
        exit(-1);
    }

    int listenerfd;
    int fd_count = 0;
    int fd_size = MAX_CONN + 2; // 5 inc connections + listener and stdin
    struct pollfd *pfds = malloc(sizeof(struct pollfd) * fd_size);

    char* username = argv[1];

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
                } else if(pfds[i].fd == FD_STDIN) { // if data is from stdin then send to client
                    int msg_len;

                    memset(&buf, 0, sizeof buf);
                    fgets(buf, sizeof(buf), stdin);
                    if (buf[strlen(buf) - 1] == '\n') {
                        buf[strlen(buf) - 1] = '\0';
                    }

                    char *targetfd_str = strtok(buf, " ");
                    int targetfd = atoi(targetfd_str);
                    if (targetfd == 0) {
                        printf("invalid target fd %s. must be a nonzero int\n", targetfd_str);
                        continue;
                    }
                    if (!valid_client_fd(pfds, targetfd, fd_size)) {
                        printf("fd %d does not exist\n", targetfd);
                        continue;
                    }
                    char *msg = strtok(NULL, "");
                    char *ccp_msg = concat(username, msg);

                    printf("send to fd %d: %s\n", targetfd, msg);
                    msg_len = strlen(ccp_msg);
                    send(targetfd, ccp_msg, msg_len, 0);

                    free(ccp_msg);
                } else { // if data is from client socket then display
                    memset(&buf, 0, sizeof buf);

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
