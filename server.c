#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(void)
{
    int sockfd = 0, r = 0;
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

    int fd = 0;
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
