#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int sockfd, connfd;
    struct sockaddr_in sockaddr;
    struct hostent *server;

    char* msg = "connected!!!!!";
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8888);
    server = gethostbyname("localhost");
    memcpy((char *)&sockaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)))) {
        fprintf(stderr, "failed to bind\n");
        perror("ERROR");
    }

    if (0 > (listen(sockfd, 10))) {
        fprintf(stderr, "failed to listen\n");
        perror("ERROR");
    }

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        write(connfd, msg, strlen(msg));
        sleep(1);
        close(connfd);
    }

    close(sockfd);
    return 0;
}
