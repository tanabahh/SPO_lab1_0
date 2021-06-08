#include "xfs.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>

#define SUPERBLOCK_START 0x00000
#define	XFS_SB_MAGIC		0x58465342	/* 'XFSB' */

void read_meta(super_xfs *fs, char *path_to_fs) {
    fs->f = fopen(path_to_fs, "rb");
    fseek(fs->f, SUPERBLOCK_START, SEEK_CUR);
    fread(&fs->meta, sizeof(xfs_sb_t), 1, fs->f);
    fs->current_dir = be64toh(fs->meta.sb_rootino);
}

int check_fs(super_xfs *fs) {
    if (XFS_SB_MAGIC == be32toh(fs->meta.sb_magicnum)) {
        return 1;
    }
    return 0;
}

//read blocks
static xfs_ino_t inode_number_(xfs_dir2_inou_t u, int is64) {
    if (is64) {
        return be64toh(*(__uint64_t *)u.i8.i);
    } else {
        return be32toh(*(__uint32_t *)u.i4.i);
    }
}

typedef struct fm_xfs_dir_iter_block_self {
    struct super_xfs *fm;
    void *entry_self;
    callback_t entry_callback;
} block_dir_iter_t;

//read directory

static node_t* xfs_dir(super_xfs *fs, xfs_dinode_core_t *core, void *dfork) {
    xfs_dir2_sf_t *sf = dfork;
    xfs_ino_t parent = inode_number_(sf->hdr.parent, sf->hdr.i8count);
    fm_xfs_dir_entry_t entry;

    entry.namelen = 1;
    entry.name[0] = '.';
    entry.name[1] = '\0';
    entry.inumber = fs->current_dir;
    entry.ftype = XFS_DIR3_FT_DIR;
    node_t* entry_list = create_list_node(entry);
    for (int i = 0; i < entry.namelen; i++) {
        printf("%c", entry.name[i]);
    }
    printf("\n");

    entry.namelen = 2;
    entry.name[0] = '.';
    entry.name[1] = '.';
    entry.name[2] = '\0';
    entry.inumber = parent;
    entry.ftype = XFS_DIR3_FT_DIR;

    push_to_list(entry_list, entry);

    for (int i = 0; i < entry.namelen; i++) {
        printf("%c", entry.name[i]);
    }
    printf("\n");

    xfs_dir2_sf_entry_t *iter = (xfs_dir2_sf_entry_t *)((char *)&sf->hdr.parent + (sf->hdr.count ? sizeof(sf->hdr.parent.i4)
            : sizeof(sf->hdr.parent.i8)));

    for (int i = 0; i < sf->hdr.count || i < sf->hdr.i8count; ++i) {
        entry.namelen = iter->namelen;
        memcpy(entry.name, iter->name, entry.namelen);
        entry.name[entry.namelen] = 0;

        xfs_dir2_sf_entry_footer_t *footer = (xfs_dir2_sf_entry_footer_t *)((char *)iter
                + offsetof(xfs_dir2_sf_entry_t, name) + iter->namelen);

        entry.ftype = footer->ftype;
        entry.inumber = inode_number_(footer->inumber, 0);

        push_to_list(entry_list, iter);

        for (int i = 0; i < iter->namelen; i++) {
            printf("%c", iter->name[i]);
        }
        printf("\n");

        iter = (xfs_dir2_sf_entry_t *)((char *)footer + offsetof(xfs_dir2_sf_entry_footer_t, inumber)
                + (sf->hdr.count ? sizeof(xfs_dir2_ino4_t) : sizeof(xfs_dir2_ino8_t)));
    }
    return 0;
}

//block read

static int xfs_dir_block (struct super_xfs *fm, xfs_dinode_core_t *core, void *dfork) {
    block_dir_iter_t block_self;
    block_self.fm = fm;

    struct xfs_bmbt_rec *list = dfork;
    struct xfs_bmbt_irec unwrapped;
    void *data = alloca(be32toh(fm->meta.sb_blocksize));

    int is_running = 1;
    xfs_filblks_t read_blocks = 0;
    for (xfs_extnum_t i = 0; is_running && i < be32toh(core->di_nextents); ++i) {
        xfs_bmbt_disk_get_all(&list[i], &unwrapped);
        fseek(fm->f, unwrapped.br_startblock * be32toh(fm->meta.sb_blocksize),
              SEEK_SET);
        for (int j = 0; is_running && unwrapped.br_startoff <= read_blocks &&
                        j < unwrapped.br_blockcount;
             ++j) {
            if (fread(data, be32toh(fm->meta.sb_blocksize), 1, fm->f) != 1)
                return 1;
            ++read_blocks;
            fm_xfs_dir_entry_t *entry = data;
            for (int i = 0; i < entry->namelen; i++) {
                printf("%c", entry->name[i]);
            }
            printf("\n");
        }
    }
    return 0;
}


static block_dir_iter_t fm_xfs_dir_iter_btree_(struct super_xfs *fm,
                                               xfs_dinode_core_t *core, void *dfork,
                                               void *self, callback_t callback) {
    block_dir_iter_t block_self;
    block_self.fm = fm;
    block_self.entry_self = self;
    block_self.entry_callback = callback;
    return block_self;
}




int xfs_ls(super_xfs *fs) {
    __uint16_t inodesize = be16toh(fs->meta.sb_inodesize);
    void *inode_info = alloca(inodesize);
    fseek(fs->f, inodesize * fs->current_dir, SEEK_SET);
    if (fread(inode_info, inodesize, 1, fs->f) != 1)
        printf("errror");
    xfs_dinode_core_t *di = inode_info;
    if (be16toh(di->di_magic) != XFS_DINODE_MAGIC)
        printf("not xfs dinode");

    void *dfork = (void *)((char *)inode_info + xfs_dinode_size(di));

    switch (di->di_format) {
        case XFS_DINODE_FMT_LOCAL:
            printf("local\n");
            xfs_dir(fs, di, dfork);
            break;
        case XFS_DINODE_FMT_EXTENTS:
            printf("extents\n");
            xfs_dir_block(fs, di, dfork);
            break;
        case XFS_DINODE_FMT_BTREE:
            printf("dtree");
            //fm_xfs_dir_iter_btree_(fs, di, dfork);
            break;
    }
}

int xfs_cd(struct super_xfs *fs) {
    char *path;
    scanf("%ms", &path);
    size_t pathlen = strlen(path);
    if (pathlen == 1 && path[0] == '/') {
        fs->current_dir = be64toh(fs->meta.sb_rootino);
        return 1;
    }
    fm_xfs_dir_entry_t dir;
    memcpy(dir.name, path, pathlen);
    dir.namelen = pathlen;
    //найти и заменить fs
}

static int is_self_or_parent_(char const *name) {
    return strcmp(".", name) == 0 || strcmp("..", name) == 0;
}

int xfs_cp(struct super_xfs *fs) {
    char *from, *to;
    scanf("%ms %ms", &from, &to);
    char *old_path = getcwd(NULL, 0);
    if (!is_self_or_parent_(to)) {
        if (mkdir(to, 0777) || chdir(to)) {
            free(old_path);
            return 0;
        }
    }
}