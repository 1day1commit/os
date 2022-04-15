#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

struct Student {
    int ID;
    char Name[10];
};

int main() {
    int i;
    struct Student student[5] = { 0 };
    char *Names[5] = {"Arnab", "Changwoo", "Na Young", "Gürkan", "Xinyu"};
    int fd[2];
    if(pipe(fd)) {
        printf("pipe failed\n");
        return 1;
    }
    switch(fork()) {
        case -1:
            printf("Fork failed.");
            return 1;
            break;
        case 0:
            close(fd[0]);
            printf("Child Process: \n");
            for (i = 0; i < 5; i++) {
                student[i].ID = i + 1;
                strcpy(student[i].Name, Names[i]);
                printf("Student name is: %s, ID is: %d\n", student[i].Name, student[i].ID);
                
            }
            write(fd[1], &student, sizeof(student));
            printf("Child Process has been completed.\n");
            break;
        
        default:
            wait(NULL);
            close(fd[1]);
            printf("\nParent Process Print\n");
            read(fd[0], &student, sizeof(student));
            for (i = 0; i < 5; i++) {
                
                printf("Student name is: %s, ID is: %d\n", student[i].Name, student[i].ID);
            }
            break;
    }
    return 0;
}
