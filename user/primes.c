#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void func(int *input, int num) {
    if (num == 1) {
        printf("prime %d\n", *input);
        return;
    }
    int p[2];
    int prime = *input;
    int tmp;
    printf("prime %d\n", prime);
    pipe(p);
    if (fork() == 0) {
        // 第一个子进程
        for (int i = 0; i < num; ++i) {
            // num 表示传递的数字的个数，input 表示传递的数组首元素的地址
            tmp = *(input + i);
            write(p[1], (char *)(&tmp), 4);
        }
        exit(0);
    }
    close(p[1]);
    if (fork() == 0) {
        int cnt = 0;
        char buf[4];
        while (read(p[0], buf, 4) != 0) {
            tmp = *(int *)buf;
            if (tmp % prime != 0) { // 过滤掉包括 prime 在内的数
                ++cnt;
                *input = tmp;
                ++input; // 更新 input 数组的值
            }
        }
        func(input - cnt, cnt);
        exit(0);
    }
    wait(0);
    wait(0);
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "there should not be argument\n");
    }
    int input[34];
    for (int i = 0; i < 34; ++i) {
        input[i] = i + 2;
    }
    func(input, 34);
    exit(0);
}