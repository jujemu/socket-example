#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 50

int main(void)
{
    int sockfd = 0;
    struct sockaddr_in ai;
    int ai_len = sizeof(ai);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    ai.sin_family = AF_INET;
    ai.sin_addr.s_addr = INADDR_ANY;
    ai.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&ai, sizeof(ai)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 0) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d...\n", PORT);

    /* select */
    int max_fd = sockfd;
    int fds[MAX_CLIENTS] = { 0 };
    int activity = 0;
    fd_set read_fds, tmp_fds;
    struct timeval timeout = { 0 };

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    int fd = 0;
    while (1) {
        tmp_fds = read_fds;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        activity = select(max_fd + 1, &tmp_fds, NULL, NULL, &timeout);

        if (activity < 0 ) {
            perror("Select failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        if (activity == 0) {
            printf("Timeout...\n");
            continue;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == sockfd) {
                    if ((fd = accept(sockfd, (struct sockaddr *)&ai, (socklen_t *)&ai_len)) < 0) {
                        perror("Accept failed");
                        close(sockfd);
                        exit(EXIT_FAILURE);
                    }
                    printf("Accept: %d socket fd...\n", fd);
                    
                    FD_SET(fd, &read_fds);
                    if (fd > max_fd) {
                        max_fd = fd;
                    }
                } else {
                    printf("Data received on socket fd %d...\n", i);
                    FD_CLR(i, &read_fds);
                }
            }
        }
    }


    if ((fd = accept(sockfd, (struct sockaddr *)&ai, (socklen_t *)&ai_len)) < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Accept: %d socket fd...\n", fd);

    close(fd);
    close(sockfd);
    return 0;
}
