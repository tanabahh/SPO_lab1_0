#include <stdio.h>
#include <string.h>

#include <getopt.h>

#include "Devices.h"
#include "xfs.h"

int main() {
    char command[80];
    scanf("%s", command);
    if (!strcmp(command, "--first")) {
        fsInfo info;
        Init(&info);
        DeviceList(&info);
        IteratorDevice(&info);
    } else if (!strcmp(command, "--second")) {
        char path[80];
        scanf("%s", path);
        super_xfs fs;
        read_meta(&fs, path);
        if (check_fs(&fs) == 1){
            char *cmd;
            scanf("%ms", &cmd);
            if (strcmp("ls", cmd) == 0)
                xfs_ls(&fs);
            else if (strcmp("cd", cmd) == 0)
                xfs_cd(&fs);
            else if (strcmp("cp", cmd) == 0)
                xfs_cp(&fs);
            else
                printf("Unknown command '%s'\n", cmd);
        } else {
            puts("Not xfs");
        }
    } else {
        puts("invalid argument\n");
        return 1;
    }
    return 0;
}
