#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>


int main()
{
    int sockfd;
    struct sockaddr_in sockaddr;
    struct hostent *server;

    server = gethostbyname("localhost");
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8888);
    memcpy((char *)&sockaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR socket:");
        return 1;
    }

    if (0 > (connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)))) {
        fprintf(stderr, "errno %d\n", errno);
        perror("ERROR connect:");
        return 1;
    }

    char buffer[64];
    int n;
    n = read(sockfd, buffer, 64);
    printf("%s\n", buffer);

    return 0;
}
