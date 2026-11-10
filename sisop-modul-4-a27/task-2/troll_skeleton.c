/*
 * troll_skeleton.c
 *
 * Starter code for "The Plagiarist's Trap".
 * Complete the TODOs and fix the BUGs.
 *
 * Build:   gcc -Wall troll_skeleton.c -o troll `pkg-config fuse --cflags --libs`
 * Mount:   sudo ./troll /mnt/honeypot
 * Unmount: sudo fusermount -u /mnt/honeypot
 */
#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

static const char *file1       = "/secret_solution.txt";
static const char *file2       = "/submit.txt";
static const char *flag_path   = "/var/tmp/honeypot_trigger.flag";
static const char *log_path    = "/var/log/honeypot.log";
static const char *target_user = "CodeCopier";
static const char *ascii_art =
"  ____  ___ _____ ____ _   _    _    _ \n"
" / ___|/ _ \\_   _/ ___| | | |  / \\  | |\n"
"| |  _| | | || || |   | |_| | / _ \\ | |\n"
"| |_| | |_| || || |___|  _  |/ ___ \\|_|\n"
" \\____|\\___/ |_| \\____|_| |_/_/   \\_(_)\n"
"\n"
"GOTCHA!! Caught red-handed!\n"
"ANJAY SLEVEW GWEH JAGO BET MAKASIH BAIK BAIK STACKOVERFLOW MWEHHEHEHEHEH\n";


static int is_triggered(void) {
    struct stat st;
    return (stat(flag_path, &st) == 0);
}


static int set_triggered(void) {
    int fd = open(flag_path, O_CREAT | O_WRONLY, 0644);
    if (fd < 0) return -errno;
    if (close(fd) < 0) return -errno;
    return 0;
}


static int is_txt_file(const char *path) {
    const char *dot = strrchr(path, '.');
    return (dot && strcmp(dot, ".txt") == 0);
}


static int is_submit_trigger(const char *buf, size_t size, off_t offset) {
    const char *cmd = "submit";
    size_t cmd_len = strlen(cmd), i = 0;

    if (offset != 0 || size < cmd_len) return 0;
    if (memcmp(buf, cmd, cmd_len) != 0) return 0;

    i = cmd_len;
    while (i < size && (buf[i] == '\n' || buf[i] == '\r')) i++;
    return (i == size);
}


// Task E
static int log_operation(const char *username, const char *operation, const char *filename) {
    time_t now = time(NULL);
    struct tm tm_info;
    char timebuf[32], line[512];
    int fd = -1, err = 0, written = 0;

    if (!localtime_r(&now, &tm_info)) return -EIO;
    if (strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm_info) == 0) return -EIO;

    fd = open(log_path, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) return -errno;

    written = snprintf(line, sizeof(line), "[%s] %s %s %s\n", timebuf, username, operation, filename);
    if (written < 0 || (size_t)written >= sizeof(line)) {
        err = -EIO;
        goto out;
    }

    if (write(fd, line, (size_t)written) != written) {
        err = (errno != 0) ? -errno : -EIO;
        goto out;
    }

out:
    if (close(fd) < 0 && err == 0) err = -errno;
    return err;
}


static int troll_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(*stbuf));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode  = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }
    if (strcmp(path, file1) == 0 || strcmp(path, file2) == 0) {
        stbuf->st_mode  = S_IFREG | 0644;
        stbuf->st_nlink = 1;
        stbuf->st_size  = 1024;
        return 0;
    }
    return -ENOENT;
}


static int troll_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    if (strcmp(path, "/") != 0) return -ENOENT;

    filler(buf, ".",  NULL, 0);
    filler(buf, "..", NULL, 0);

    // Task B
    filler(buf, "secret_solution.txt", NULL, 0);
    filler(buf, "submit.txt", NULL, 0);

    return 0;
}


static int troll_open(const char *path, struct fuse_file_info *fi) {
    if (strcmp(path, file1) == 0 || strcmp(path, file2) == 0) return 0;
    return -ENOENT;
}


static int troll_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    uid_t uid = fuse_get_context()->uid;
    struct passwd *pw = getpwuid(uid);
    const char *user = (pw && pw->pw_name) ? pw->pw_name : "unknown", *filename = (path[0] == '/') ? path + 1 : path;
    int log_rc = 0;

    if (strcmp(path, file1) != 0 && strcmp(path, file2) != 0) return -ENOENT;

    // Task E
    log_rc = log_operation(user, "READ", filename);
    if (log_rc != 0) return log_rc;
    const char *data = "";

    // Task D
    if (is_triggered() && is_txt_file(path)) data = ascii_art;

    // Task C
    if (data[0] == '\0' && strcmp(path, file1) == 0) {
        if (strcmp(user, target_user) == 0) data = "Algorithm walkthrough: Use dynamic programming with memoization on overlapping subproblems\n";
        else data = "CodeCopier's plagiarized assignments archive!!.txt\n";
    }

    size_t len = strlen(data);
    if (offset < (off_t)len) {
        if (offset + size > len) size = len - offset;
        memcpy(buf, data + offset, size);
    } else size = 0;
    return size;
}


static int troll_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    uid_t uid = fuse_get_context()->uid;
    struct passwd *pw = getpwuid(uid);
    const char *user = (pw && pw->pw_name) ? pw->pw_name : "unknown", *filename = (path[0] == '/') ? path + 1 : path;
    int log_rc = 0;

    if (strcmp(path, file2) != 0) return -EACCES;

    // Task E
    log_rc = log_operation(user, "WRITE", filename);
    if (log_rc != 0) return log_rc;

    // Task D
    if (!is_triggered() && is_submit_trigger(buf, size, offset)) {
        int rc = set_triggered();
        if (rc != 0) return rc;
    }
    return size;
}


static int troll_truncate(const char *path, off_t size) {
    if (strcmp(path, file2) == 0) return 0;
    return -EACCES;
}


static struct fuse_operations troll_oper = {
    .getattr  = troll_getattr,
    .readdir  = troll_readdir,
    .open     = troll_open,
    .read     = troll_read,
    .write    = troll_write,
    .truncate = troll_truncate,
};


int main(int argc, char *argv[]) {
    umask(0);

    int new_argc = 0;
    char **new_argv = malloc(sizeof(char *) * (argc + 4));
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = "-f";
    new_argv[new_argc++] = "-o";
    new_argv[new_argc++] = "allow_other";
    if (argc > 1) new_argv[new_argc++] = argv[1];

    int ret = fuse_main(new_argc, new_argv, &troll_oper, NULL);
    free(new_argv);
    return ret;
}