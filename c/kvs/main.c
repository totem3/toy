#include <stdio.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_LENGTH 100
int table[100];

int hash(char *str)
{
    int n = 0;
    for (unsigned long l = 0; l < strlen(str); l++) {
        n += str[l] << l;
    }
    return n % MAX_LENGTH;
}

int get(int *table, char *key)
{
    int key_hash = hash(key);
    printf("get %d\n", key_hash);
    return table[key_hash];
}

void set(int *table, char *key, int value)
{
    int key_hash = hash(key);
    printf("set %d\n", key_hash);
    table[key_hash] = value;
}

void thread_func(int *sockfd)
{
    int fd = accept(*sockfd, NULL, NULL);
    if (fd < 0) {
        perror("failed to accept");
        return;
    } else {
        //fprintf(stderr, "connect\n");
    }
    char mes[64];
    read(fd, mes, sizeof(mes));
    char *pos = strchr(mes, ' ');

    int length = pos-mes;
    char *cmd = (char *) malloc(length);
    memcpy(cmd, mes, length);
    if (0 == strcmp(cmd, "get")) {
        char *postpos = strchr(pos, '\n');
        char *key = (char *) malloc(length);
        int length = postpos-pos;
        memcpy(key, pos+1, length-1);
        int val = get(table, key);
        char v[32];
        sprintf(v, "%d", val);
        write(fd, v, strlen(v));
    } else if (0 == strcmp(cmd, "set")) {
        char *postpos = strchr(pos+1, ' ');
        int length = postpos-pos;
        char *key = (char *) malloc(length);
        memcpy(key, pos+1, length-1);

        char *pppos = strchr(postpos, '\0');
        int length2 = pppos-postpos;
        char *key2 = (char *) malloc(length2);
        char val[16];
        memcpy(val, postpos+1, length2-1);

        set(table, key, atoi(val));
        char *ok = "ok";
        write(fd, ok, strlen(ok));
    } else {
        char *failure = "unknown command";
        write(fd, failure, strlen(failure));
    }
    free(cmd);
    close(fd);

}

int main()
{
    int sockfd;
    int fd;
    struct hostent *host;
    struct sockaddr_in addr;

    host = gethostbyname("localhost");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* memcpy(addr.sin_addr->s_addr, host->h_addr_list[0], host->h_length); */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (0 > bind(sockfd, (struct sockaddr *) &addr, sizeof(addr))) {
        perror("failed to bind");
        return 1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
    if (0 > listen(sockfd, 10)) {
        perror("failed to listen");
        return 1;
    } else {
        fprintf(stderr, "listen start\n");
    }

    while (1) {
        pthread_t thread;
        if (pthread_create(&thread, NULL, thread_func, &sockfd) != 0) {
            perror("thread failed");
        }
        pthread_join(thread, NULL);
    }

    close(sockfd);
    return 0;
}
