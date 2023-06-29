#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "there should not be argument");
    }
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);
    int pid = fork();
    if (pid == 0) {
        close(p1[1]);
        close(p2[0]);
        char recv[2] = {0};
        read(p1[0], recv, 1);
        printf("%d: received ping\n");
        write(p2[1], recv, 1);
        exit(0);
    }
    // 说明是父进程
    close(p1[0]);
    close(p2[1]);
    char *msg = "a\0";
    write(p1[1], msg, 1);
    read(p2[0], msg, 1);
    wait((int *)0);
    printf("%d: received pong\n", getpid());
    exit(0);
}