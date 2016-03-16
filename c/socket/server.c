#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void sig_catch(int sig) {
    pid_t pid_child = 0;
    do {
        int ret_child;
        pid_child = waitpid(-1, &ret_child, WNOHANG);
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
        fprintf(stderr, "failed to bind\n");
        perror("ERROR");
    }

    if (0 > (listen(sockfd, 10))) {
        fprintf(stderr, "failed to listen\n");
        perror("ERROR");
    }

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        pid_t pid = fork();
        if (pid < 0) {
            perror("failed to fork. ");
            /* break; */
        } else if (pid == 0) {
            write(connfd, msg, strlen(msg));
            sleep(1);
            close(connfd);
            break;
        }
    }

    close(sockfd);
    return 0;
}
