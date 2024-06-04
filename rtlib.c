#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFSIZE 1024
int check_proc_map(unsigned long addr)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t rc;
    unsigned long start, end;
    char perms[BUFSIZE];
    int r, w, x, p;
    int found;

    fp = fopen("/proc/self/maps", "r");
    if (!fp) {
        perror("fopen");
        exit(-1);
    }

    found = 0;
    while ((rc = getline(&line, &len, fp)) != -1) {
        sscanf(line, "%lx-%lx %s", &start, &end, perms);
        r = perms[0] == 'r';
        w = perms[1] == 'w';
        x = perms[2] == 'x';
        p = perms[3] == 'p';
        if (addr >= start && addr <= end) {
            found = 1;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "vtable addr %lx not found in /proc/self/maps\n", addr);
        exit(1);
    }

    /* check if the segment containning addr has write permission */
    if (w) {
        fprintf(stderr, "vtable addr %lx have write permission!!!!\n", addr);
        fprintf(stderr, "aborting the execution......\n");
        exit(1);
    }
    return 0;
}
int check_permission(unsigned long addr)
{
    return check_proc_map(addr);
}
void trace(unsigned long addr)
{
    // printf("VTable call detected!\n");
    // printf("VTable addr is %lx\n", addr);
    check_permission(addr);
}

