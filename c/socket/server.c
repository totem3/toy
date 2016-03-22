#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_SERVERS 5
#define MAX_REQUESTS 100
int slot = 0;
int graceful = 0;

void sig_catch(int sig) {
    if (sig == SIGINT) {
        graceful = 1;
        return;
    }
    pid_t pid_child = 0;
    do {
        int ret_child;
        pid_child = waitpid(-1, &ret_child, WNOHANG);
        if (pid_child > 0) {
            slot--;
        }
    } while (pid_child > 0);
}

int main()
{

    struct sigaction act;
    act.sa_handler = sig_catch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    if (0 > sigaction(SIGCHLD, &act, NULL)) {
        perror("failed to set sigaction: ");
    }
    if (0 > sigaction(SIGINT, &act, NULL)) {
        perror("failed to set sigaction: ");
    }

    int sockfd, connfd;
    int status;
    struct sockaddr_in sockaddr;
    struct hostent *server;

    char* msg = "connected!!!!!";
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(8888);
    server = gethostbyname("localhost");
    memcpy((char *)&sockaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)))) {
        perror("failed to bind");
    }

    if (0 > (listen(sockfd, 10))) {
        perror("failed to listen");
    }

    while (1) {
        usleep(100);
        if (graceful && slot == 0) break;
        if (slot >= MAX_SERVERS || graceful) continue;

        pid_t pid = fork();
        int req = 0;
        if (pid < 0) {
            perror("failed to fork. ");
        } else if (pid == 0) {
            pid_t selfpid = getpid();
            while (!graceful) {
                connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
                char send[32];
                if (0 < sprintf(send, "[%d] %s", selfpid, msg)) {
                    write(connfd, send, strlen(send));
                } else {
                    write(connfd, msg, strlen(msg));
                }
                usleep(100);
                close(connfd);
                if (req++ > MAX_REQUESTS) {
                    fprintf(stderr, "[%d] max requests...\n", selfpid);
                    break;
                }
            }
            if (graceful) fprintf(stderr, "[%d] graceful finishing...\n", selfpid);
            break;
        } else {
            slot++;
        }
    }

    close(sockfd);
    return 0;
}
