#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    pid_t child = fork();
    char *argv1[] = {"unzip", "evidence.zip", "-d", "logs_dump", NULL};
    char *argv2[] = {"rm", "evidence.zip", NULL};

    if(child == 0) execv("/bin/unzip", argv1);
    else{
        wait(NULL);
        execv("/bin/rm", argv2);
    }
    return 0;
}