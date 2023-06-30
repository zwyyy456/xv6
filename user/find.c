#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *fmtname(char *path) { // path 可能是绝对路径
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));                     // memmove(dst, src, len);
    memset(buf + strlen(p), 0, DIRSIZ - strlen(p)); // 后面全部置为空
    return buf;
}

void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct stat st; // 文件状态
    struct dirent de;
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        exit(1);
    }
    switch (st.type) {
    case T_FILE:
        if (strcmp(name, fmtname(path)) == 0) {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/'; // 让路径以 '/' 结尾
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || de.inum == 1 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            find(buf, name);
        }
        break;
    }
    close(fd);
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "syntax should be like : find <path> <filename>");
    }
    char *path = argv[1]; // 要搜索的路径
    char *name = argv[2]; // 要查找的名字
    find(path, name);
    exit(0);
}