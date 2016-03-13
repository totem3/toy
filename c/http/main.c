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

    char scheme[8];
    memset(scheme, 0, 8);
    char hostname[256];
    memset(hostname, 0, 256);
    char path[256];
    memset(path, 0, 256);

    if (argc < 1) {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    printf("sock open: %d\n", sockfd);

    // parse argv[1]
    int ret = sscanf(argv[1],
                   "%8[^\n:]://%[^\n/?]%[^\n]",
                   scheme,
                   hostname,
                   path);

    if (ret < 3) {
        memset(scheme, 0, 8);
        strcpy(scheme, "http");
        memset(hostname, 0, 256);
        memset(path, 0, 256);
        ret = sscanf(argv[1], "%[^\n/?]%[^\n]", hostname, path);
    }
    ret = sscanf(hostname, "%[^\n:/?]:%d", hostname, &portno);
    if (path[0] == 0) {
        path[0] = '/';
    }
    if (portno == 0) {
        portno = 80;
    }
    printf("scheme: %s\n", scheme);
    printf("hostname: %s\n", hostname);
    printf("path: %s\n", path);
    printf("portno: %d\n", portno);

    server = gethostbyname(hostname);
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
    char message[100];
    sprintf(message, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", hostname);
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
