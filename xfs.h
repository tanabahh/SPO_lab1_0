//
// Created by taisia on 13.03.2021.
//

#ifndef SPO_LAB1_0_XFS_H
#define SPO_LAB1_0_XFS_H
#include <bits/stdint-intn.h>
#include <stdint.h>
#include <sys/types.h>
#include "xfs_types.h"
/* 8-битная длина файла -- максимум 256 символов */
#define FILENAME_BUFLEN 256
//#include </usr/include/xfs/handle.h>
//#include </usr/include/xfs/jdm.h>
//#include </usr/include/xfs/linux.h>
//#include </usr/include/xfs/xfs_arch.h>
//#include </usr/include/xfs/xfs_fs_compat.h>
//#include </usr/include/xfs/xfs_types.h>
//#include </usr/include/xfs/xqm.h>
//#include </usr/include/xfs/xfs.h>
//#include </usr/include/xfs/xfs_format.h>
//#include </usr/include/xfs/xfs_log_format.h>
//#include </usr/include/xfs/xfs_da_format.h>
//#include </usr/include/xfs/xfs_fs.h>

typedef struct super_xfs {
    FILE *f;
    xfs_sb_t meta;
    xfs_ino_t current_dir;
} super_xfs;

typedef int (*callback_t)(void *, void *);

typedef struct fm_xfs_dir_entry {
    ino_t inumber;
    char name[FILENAME_BUFLEN];
    __uint8_t namelen;
    __uint8_t ftype;
} fm_xfs_dir_entry_t;

int check_fs(super_xfs *fs);
void read_meta(super_xfs *fs, char * path_to_fs);
void print_meta(void);
int xfs_ls(super_xfs *fs);
int xfs_cd(super_xfs *fs);
int xfs_cp(super_xfs *fs);
#endif //SPO_LAB1_0_XFS_H
