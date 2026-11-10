#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#define MAX 512

FILE *log_file;
char file_pid[MAX];
char file_log[MAX];
char honeypot[MAX];
char quarantine[MAX];

volatile sig_atomic_t running = 1;

void convert_time(char *timestamp){
    time_t now = time(NULL);
    struct tm *waktu = localtime(&now);
    strftime(timestamp, 50, "%Y-%m-%d %H:%M:%S", waktu);
}

void handle_sigterm(int sigterm){
    unlink(file_pid);
    exit(EXIT_SUCCESS);
}

int check_file(char *name){
    return (strstr(name, ".exe") || strstr(name, ".pcap"));
}

int main(){
    char path[256];
    if(getcwd(path, sizeof(path)) == NULL){
        exit(EXIT_FAILURE);
    }
    sprintf(file_pid, "%s/watcher.pid", path);
    sprintf(file_log, "%s/security.log", path);
    sprintf(honeypot, "%s/honeypot", path);
    sprintf(quarantine, "%s/quarantine", path);

    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"mkdir", "-p", honeypot, NULL};
        execv("/bin/mkdir", argv);
    }
    pid_t pid2 = fork();
    if(pid2 == 0){
        char *argv[] = {"mkdir", "-p", quarantine, NULL};
        execv("/bin/mkdir", argv);
    }
    pid_t pid3 = fork();
    if(pid3 < 0) exit(EXIT_FAILURE);
    if(pid3 > 0) exit(EXIT_SUCCESS);

    setsid();
    umask(0);
    chdir("/"); 

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    FILE *pid_file = fopen(file_pid, "w");
    if(pid_file){
        fprintf(pid_file, "%d", getpid());
        fclose(pid_file);
    }

    log_file = fopen(file_log, "a");

    signal(SIGTERM, handle_sigterm);

    while(1){
        DIR *directory = opendir(honeypot);
        if(!directory){
            sleep(1);
            continue;
        }
        struct dirent *note;
        while((note = readdir(directory)) != NULL){
            if(strcmp(note->d_name, ".") == 0 || strcmp(note->d_name, "..") == 0) continue;
            if(!check_file(note->d_name)) continue;

            char source[1000], destination[1000];
            sprintf(source, "%s/%s", honeypot, note->d_name);
            sprintf(destination, "%s/%s", quarantine, note->d_name);

            char timestamp[50];
            convert_time(timestamp);
            fprintf(log_file, "[%s] Peringatan! Menemukan file mencurigakan: %s\n", timestamp, note->d_name);
            fflush(log_file);

            pid_t pid = fork();
            if(pid == 0){
                char *argv[] = {"mv", source, destination, NULL};
                execv("/bin/mv", argv);
            }

            convert_time(timestamp);
            fprintf(log_file, "[%s] Berhasil mengkarantina file: %s\n", timestamp, note->d_name);
            fflush(log_file);
        }
        sleep(1);
        closedir(directory);
    }
    return 0;
}