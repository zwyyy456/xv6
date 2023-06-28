#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "command should be like \'pingpong arg\'");
    }
    int p1[2], p2[2];
    int pid = fork();
    pipe(p1);
    pipe(p2);
    if (pid > 0) {
        // 说明是父进程
        write(p1, argv[1], 1);
        wait();
    }
}