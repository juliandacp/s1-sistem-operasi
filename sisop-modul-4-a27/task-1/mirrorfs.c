#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/bagas/reports";

static int xmp_getattr(const char *path, struct stat *stbuf){
    int res;
    char fpath[4096];

    if(strncmp(path, "/LAPORAN_", 9) == 0) snprintf(fpath, sizeof(fpath), "%s/%s", dirpath, path + 9);
    else snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    if(strcmp(path, "/") != 0){
        char *extension = strrchr(fpath, '.');
        if(extension == NULL || strcmp(extension, ".txt") != 0) return -ENOENT;
    }

    res = lstat(fpath, stbuf);

    if(res == -1) return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    char fpath[4096];

    if(strcmp(path, "/") == 0) snprintf(fpath, sizeof(fpath), "%s", dirpath);
    else snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if(dp == NULL) return -errno;

    while((de = readdir(dp)) != NULL){
        struct stat st;
        char new_name[4096];

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) res = filler(buf, de->d_name, &st, 0);
        else{
            char *extension = strrchr(de->d_name, '.');
            if(extension != NULL && strcmp(extension, ".txt") == 0){
                snprintf(new_name, sizeof(new_name), "LAPORAN_%s", de->d_name);
                res = filler(buf, new_name, &st, 0);
            }
        }
        if(res != 0) break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char fpath[4096];

    if(strncmp(path, "/LAPORAN_", 9) == 0) snprintf(fpath, sizeof(fpath), "%s/%s", dirpath, path + 9);
    else if(strcmp(path, "/") == 0) snprintf(fpath, sizeof(fpath), "%s", dirpath);
    else snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    char *extension = strrchr(fpath, '.');
    if(extension == NULL || strcmp(extension, ".txt") != 0) return -ENOENT;

    int res = 0;
    int fd = 0;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if(fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if(res == -1) res = -errno;

    close(fd);

    return res;
}

// Read Only
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    return -EROFS;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    return -EROFS;
}

static int xmp_mkdir(const char *path, mode_t mode){
    return -EROFS;
}

static int xmp_unlink(const char *path){
    return -EROFS;
}

static int xmp_rmdir(const char *path){
    return -EROFS;
}

static int xmp_rename(const char *from, const char *to){
    return -EROFS;
}

static int xmp_truncate(const char *path, off_t size){
    return -EROFS;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .write = xmp_write,
    .create = xmp_create,
    .mkdir = xmp_mkdir,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .truncate = xmp_truncate,
};

int main(int argc, char *argv[]){
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}