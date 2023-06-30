#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: sleep x...\n");
        exit(1);
    }
    if (argc >= 3) {
        fprintf(2, "two many arguments...\n");
        exit(1);
    }
    int time = atoi(argv[1]);
    fprintf(1, "(nothing happen for a little while)\n");
    sleep(time);
    exit(0);
}
