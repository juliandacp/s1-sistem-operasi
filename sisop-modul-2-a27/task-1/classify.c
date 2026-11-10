#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAX 512

int main(){
    DIR *directory;
    struct dirent *note;
    char attack_type[50][100];
    int attack_count = 0;

    directory = opendir("logs_dump/evidence_raw");

    while((note = readdir(directory)) != NULL){
        if(note->d_type == DT_REG){
            char file_name[MAX];
            strcpy(file_name, note->d_name);

            char *attack_name = strtok(file_name, "_");
            if(attack_name != NULL){
                int found = 0;
                for(int i = 0; i < attack_count; i++){
                    if(strcmp(attack_type[i], attack_name) == 0){
                        found = 1;
                        break;
                    }
                }
                if(!found){
                    strcpy(attack_type[attack_count], attack_name);
                    attack_count++;
                }
            }
        }
    }
    closedir(directory);

    for(int i = 0; i < attack_count; i++){
        pid_t pid = fork();
        if(pid == 0){
            char path[MAX];
            sprintf(path, "logs_dump/%s", attack_type[i]);
            pid_t pid = fork();
            if(pid == 0){
                char *argv[] = {"mkdir", "-p", path, NULL};
                execv("/bin/mkdir", argv);
            }else wait(NULL); 

            DIR *directory2 = opendir("logs_dump/evidence_raw");
            struct dirent *note2;

            while((note2 = readdir(directory2)) != NULL){
                if(note2->d_type == DT_REG){
                    char temp[MAX];
                    strcpy(temp, note2->d_name);

                    char *type_name = strtok(temp, "_");
                    if(type_name != NULL && strcmp(type_name, attack_type[i]) == 0){
                        char source[MAX];
                        char destination[MAX];
                        sprintf(source, "logs_dump/evidence_raw/%s", note2->d_name);
                        sprintf(destination, "logs_dump/%s/%s", attack_type[i], note2->d_name);

                        pid_t pid = fork();
                        if(pid == 0){
                            char *argv[] = {"mv", source, destination, NULL};
                            execv("/bin/mv", argv);
                        }else wait(NULL);
                    }
                }
            }
            closedir(directory2);
            exit(EXIT_SUCCESS);
        }
    }
    while(wait(NULL) > 0);

    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"rm", "-rf", "logs_dump/evidence_raw", NULL};
        execv("/bin/rm", argv);
    }
    return 0;
}