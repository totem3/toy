#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char const* argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    const int BUFLEN = 1024;
    char buffer[BUFLEN];
    char body[2048];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    printf("sock open: %d\n", sockfd);

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* bzero((char *) &serv_addr, sizeof(serv_addr)); */
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    printf("%d\n", *server->h_addr);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    printf("connect\n");
    memset(buffer, 0, BUFLEN);
    const char* host = argv[1];
    printf("%s(length %lu)\n", host, strlen(host));
    char message[100];
    sprintf(message, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", host);
    printf("message = %s\n", message);
    n = write(sockfd, message, strlen(message));
    printf("write %d bytes\n", n);
    if (n < 0)
        error("ERORR writing to socket");
    memset(buffer, 0, BUFLEN);
    printf("start reading\n");

    
    char b[1];
    char chk[4];
    memset(chk, 0, 4);
    int idx = 0;
    while(read(sockfd, b, 1) > 0) {
        buffer[idx++] = *b;
        strncpy(chk, &buffer[idx-4], 4);
        if (strncmp(chk, "\r\n\r\n", 4) == 0) {
            printf("DONE\n");
            break;
        }
    }
    printf("-----\n");
    printf("%s\n", buffer);
    printf("-----\n");

    memset(body, 0, 2048);
    n = read(sockfd, body, 1270);
    if (n < 0) {
        error("ERROR reading from socket");
        exit(1);
    }
    printf("read %d\n", n);
    printf("%s\n", body);
    close(sockfd);
    return 0;
}
