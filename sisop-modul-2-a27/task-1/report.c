#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct{
    char name[50];
    int count;
}Data;

Data attack[50];
int size_attack = 0;
Data ip[50];
int size_ip = 0;

int group(Data arr[], int *size, char *name){
    for(int i = 0; i < *size; i++){
        if(strcmp(arr[i].name, name) == 0) return i;
    }
    strcpy(arr[*size].name, name);
    arr[*size].count = 0;
    (*size)++;
    return (*size) - 1;
}

void sort(Data arr[], int size){
    for(int i = 0; i < size - 1; i++){
        for(int j = 0; j < size - i - 1; j++){
            if(arr[j].count < arr[j + 1].count){
                Data temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(){
    DIR *directory = opendir("logs_dump");

    struct dirent *note;
    while((note = readdir(directory)) != NULL){
        if(strcmp(note->d_name, ".") == 0 ||strcmp(note->d_name, "..") == 0) continue;

        char path[1000];
        sprintf(path, "logs_dump/%s", note->d_name);

        if (note->d_type != DT_DIR) continue;

        char *attack_type = note->d_name;

        DIR *sub_directory = opendir(path);
        if(!sub_directory) continue;

        struct dirent *note;

        while((note = readdir(sub_directory)) != NULL){
            if(note->d_type != DT_REG) continue;

            int idx_attack = group(attack, &size_attack, attack_type);
            attack[idx_attack].count++;

            char temp[50];
            strcpy(temp, note->d_name);

            char *type = strtok(temp, "_");
            char *ip_address = strtok(NULL, "_");
            if(!ip_address) continue;

            int idx_ip = group(ip, &size_ip, ip_address);
            ip[idx_ip].count++;
        }
        closedir(sub_directory);
    }
    closedir(directory);

    sort(attack, size_attack);
    sort(ip, size_ip);

    FILE *file = fopen("threat_report.txt", "w");

    fprintf(file, "Laporan Klasifikasi Serangan:\n");
    for(int i = 0; i < size_attack; i++) fprintf(file, "%d. %s muncul sebanyak %d kali.\n", i + 1, attack[i].name, attack[i].count);
    fprintf(file, "\n");
    fprintf(file, "Top 5 IP Mencurigakan:\n");
    for(int i = 0; i < size_ip && i < 5; i++) fprintf(file, "%d. %s muncul sebanyak %d kali.\n", i + 1, ip[i].name, ip[i].count);

    fclose(file);

    pid_t pid = fork();
    if(pid == 0){
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        char *argv[] = {"zip", "-r", "backup_evidence.zip", "logs_dump", "threat_report.txt", NULL};
        execv("/usr/bin/zip", argv);
    }else if(pid > 0){
        wait(NULL);
        printf("[Info] Laporan dan arsip berhasil dibuat\n");
    }
    return 0;
}