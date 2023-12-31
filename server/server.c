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

char* concat(const char *s1, const char *s2);

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
        return -1
    }

    free(res);

    return sock;
}

int add_pfd(struct pollfd pfds[], int newfd, int *fd_count, int fd_size)
{
    if (*fd_count == *fd_size) {
        return 1;
    }

    pfds[*fd_count].fd = newfd;
    pfds[*fd_count].events = POLLIN;
    (*fd_count)++;

    return 0;
}

void rm_pfd(struct pollfd pfds[], int i, int *fd_count)
{
    pfds[i].fd = pfds[*fd_count - 1].fd;
    (*fd_count)--;
}

int main() {

    int listenerfd;
    int fd_count = 0
    int fd_size = MAX_CONN + 1;
    struct pollfd *pdfs = malloc(sizeof(struct pollfd) * fd_size);

    listenerfd = get_listener_socket(PORT);
    pdfs = 




    free(pdfs);


    

    int inc_fd;
    struct sockaddr_storage inc_addr;
    socklen_t addr_size = sizeof(struct sockaddr_storage);

    char inc_msg[1024];
    char *out_msg;
    int out_msg_len;
    memset(&inc_msg, 0, sizeof inc_msg);


        inc_fd = accept(fd, (struct sockaddr *)&inc_addr, &addr_size);
        printf("new connection recvd\n");

 
            close(fd);
            // while (recv(inc_fd, inc_msg, 1024, 0)) {
            //     memset(&out_msg, 0, sizeof out_msg);
            //     char *resp = responses[rand() % sizeof(responses)/sizeof(responses[0])];
            //     out_msg = concat(inc_msg, resp);
            //     out_msg_len = strlen(out_msg);
            //     send(inc_fd, out_msg, out_msg_len, 0);
            //     free(out_msg);
            //     memset(&inc_msg, 0, sizeof inc_msg);
            // }
            recv(inc_fd, inc_msg, 1024, 0);
    while(1) {
printf("sending...\n");
                out_msg = "Hello world";
                out_msg_len = strlen(out_msg);
                send(inc_fd, out_msg, out_msg_len, 0);
                printf("sending...\n");
                sleep(3);
    }
                
                
     



            close(inc_fd);
            exit(0);

    
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
