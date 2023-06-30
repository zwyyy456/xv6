#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    int j = 0;
    int k;
    int l, m = 0;
    char block[32];
    char buf[32];
    char *p = buf;
    char *line_split[32];
    for (int i = 1; i < argc; ++i) {
        line_split[j++] = argv[i]; // 最后 j = argc - 1;
    }
    while ((k = read(0, block, sizeof(block))) > 0) {
        for (l = 0; l < k; ++l) {
            if (block[l] == '\n') {
                buf[m] = 0;
                m = 0;
                line_split[j++] = p;
                p = buf;
                line_split[j] = 0;
                j = argc - 1;
                if (fork() == 0) {
                    exec(argv[1], line_split);
                }
                wait(0);
            } else if (block[l] == ' ') {
                buf[m++] = 0; // m 表示命令行中参数个数？
                line_split[j++] = p;
                p = &buf[m];
            } else {
                buf[m++] = block[l];
            }
        }
    }
    exit(0);
}