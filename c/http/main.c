#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define DEBUG 1
#define debug_print(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while(0)

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

    debug_print("sock open: %d\n", sockfd);

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
    debug_print("scheme: %s\n", scheme);
    debug_print("hostname: %s\n", hostname);
    debug_print("path: %s\n", path);
    debug_print("portno: %d\n", portno);

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    debug_print("connect\n");
    memset(buffer, 0, BUFLEN);
    char message[128];
    sprintf(message, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: hurl/0.0.1\r\n\r\n", path, hostname);
    debug_print("message = %s\n", message);
    n = write(sockfd, message, strlen(message));
    debug_print("write %d bytes\n", n);
    if (n < 0)
        error("ERORR writing to socket");
    memset(buffer, 0, BUFLEN);
    debug_print("start reading\n");

    char b[1];
    char chk[4];
    memset(chk, 0, 4);
    int idx = 0;
    while(read(sockfd, b, 1) > 0) {
        buffer[idx++] = *b;
        strncpy(chk, &buffer[idx-4], 4);
        if (strncmp(chk, "\r\n\r\n", 4) == 0) {
            debug_print("DONE\n");
            break;
        }
    }
    printf("-----\n");
    printf("%s\n", buffer);
    printf("-----\n");

    int code = 0;
    char msg[64];
    sscanf(buffer, "HTTP/1.1 %d %s\r\n", &code, msg);
    printf("code: %d\n", code);
    printf("msg: %s\n", msg);
    char *start, *end;
    start = strchr(buffer, '\n');
    char header_key[64];
    memset(header_key, 0, 64);
    char header_value[256];
    memset(header_value, 0, 256);
    int content_length = 0;

    while (0 != (end = strchr(start, ':'))) {
        strncpy(header_key, start, end - (start));
        start = end + 1;
        end = strchr(start, '\n');
        strncpy(header_value, start, end - (start+1));
        start = end + 1;
        if (0 == strcmp(header_key, "Content-Length")) {
            content_length = atoi(header_value);
        }
        memset(header_key, 0, 64);
        memset(header_value, 0, 256);
    }

    char readbuf[16384];
    char *body;
    body = (char *)malloc(content_length);
    char *rstart;
    memset(body, 0, content_length);
    int read_length = 0;
    rstart = body;
    while (1) {
        printf("content_length: %d\n", content_length);

        printf("F_GETFL %d\n", fcntl(sockfd, F_GETFL));
        n = read(sockfd, readbuf, content_length);
        if (n < 0) {
            error("ERROR reading from socket");
            break;
        } else {
            /* int wn = write(sockfd, message, strlen(message)); */
            /* printf("write %d bytes\n", wn); */
            content_length -= n;
            printf("rest %d bytes\n", content_length);
        }
        strncpy(rstart, readbuf, n);
        rstart = rstart + n;
        if (content_length <= 0) {
            read_length += n;
            debug_print("read done\n");
            debug_print("read length: %d\n", read_length);
            break;
        }
    }
    printf("F_GETFL %d\n", fcntl(sockfd, F_GETFL));
    printf("----\n");
    printf("%s\n", body);
    free(body);
    int wn = write(sockfd, message, strlen(message));
    printf("write %d bytes\n", wn);
    while (1) {
        n = read(sockfd, buffer, 64);
        printf("read %d bytes\n", n);
        if (n <= 0) {
            break;
        }
    }
    close(sockfd);
    return 0;
}
