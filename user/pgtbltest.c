#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

void ugetpid_test();
void pgaccess_test();

int main(int argc, char *argv[]) {
    ugetpid_test();
    pgaccess_test();
    printf("pgtbltest: all tests succeeded\n");
    exit(0);
}

char *testname = "???";

void err(char *why) {
    printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
    exit(1);
}

void ugetpid_test() {
    int i;

    printf("ugetpid_test starting\n");
    testname = "ugetpid_test";

    for (i = 0; i < 64; i++) {
        int ret = fork();
        if (ret != 0) {
            wait(&ret);
            if (ret != 0)
                exit(1);
            continue;
        }
        if (getpid() != ugetpid())
            err("missmatched PID");
        exit(0);
    }
    printf("ugetpid_test: OK\n");
}

void pgaccess_test() {
    char *buf;
    unsigned int abits;
    printf("pgaccess_test starting\n");
    testname = "pgaccess_test";
    buf = malloc(32 * PGSIZE);
    printf("debug: buf addr: %p\n", buf);
    if (pgaccess(buf, 32, &abits) < 0)
        err("pgaccess failed");
    buf[PGSIZE * 1] += 1;
    buf[PGSIZE * 2] += 1;
    buf[PGSIZE * 30] += 1; // 说明这三个 page 被访问了
    if (pgaccess(buf, 32, &abits) < 0)
        err("pgaccess failed");
    if (abits != ((1 << 1) | (1 << 2) | (1 << 30))) {
        printf("target: %d\n", (1 << 1) | (1 << 2) | (1 << 30));
        printf("abits: %d\n");
        err("incorrect access bits set");
    }
    free(buf);
    printf("pgaccess_test: OK\n");
}
/*
100001111111011001010011011111
1000000
1000000000000000000000000000111
1000000000000000000000000000110
*/