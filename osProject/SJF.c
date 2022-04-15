#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


char name[8][100] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};
typedef struct Projectss {
    char team_name[4];
    char project_name[4];
    char manager[50]; // just store first char
    char member[50]; // just store first char
} projectss;

void SJF(int fd[13][2]) {
    int i, j;

    /**
     * Dummy Data
     */
    projectss p1, p2, p3, p4, p5;
    // Team_A Project_A Alan Cathy Fanny Helen
    strcpy(p1.team_name, "A");
    strcpy(p1.project_name, "A");
    strcpy(p1.manager, "A");
    strcpy(p1.member, "CFH");

    // Team_B Project_B Billy Alan Eva Gary
    strcpy(p2.team_name,"B");
    strcpy(p2.project_name, "B");
    strcpy(p2.manager, "B");
    strcpy(p2.member, "AEG");

    // Team_C Project_C Cathy Alan David Eva
    strcpy(p3.team_name,"C");
    strcpy(p3.project_name, "C");
    strcpy(p3.manager, "C");
    strcpy(p3.member, "ADE");

    // Team_D Project_D David Alan Cathy Billy
    strcpy(p4.team_name,"D");
    strcpy(p4.project_name, "D");
    strcpy(p4.manager, "D");
    strcpy(p4.member, "ACB");

    // Team_E Project_E Eva Billy David Helen
    strcpy(p5.team_name,"E");
    strcpy(p5.project_name, "E");
    strcpy(p5.manager, "E");
    strcpy(p5.member, "BDH");


    
    // 1. read input file
    FILE *fp;
    fp = fopen("input_meeting2.txt", "r");

    // 2. read project info
    // Team_A 2022-04-25 09:00 2
    // 3. store data
    char meeting_data[162][5][1024];
    char rected_meetings[162][5][1024];

    char buffer[1024]; char *token; int idx = 0;
    while (!feof(fp)){
        i = 0;
        fgets(buffer, 1024, fp);
        token = strtok(buffer, " ");
        int start = 0; int duration = 0; int end = 0;
        while (token != NULL){
            
            // store date first
            //meeting_data[idx][i] = (char *)malloc(sizeof(char) * 1024);
            if (i == 0){ // Team_A
                strcpy(meeting_data[idx][1], token);
            }
            if (i == 1){ //2022-04-25
                strcpy(meeting_data[idx][0], token);
            }
            if (i == 2){ //09:00
                strcpy(meeting_data[idx][2], token);
                start = atoi(token);
            }
            if (i == 3){ // 2
                strcpy(meeting_data[idx][3], token);
                duration = atoi(token);
            }

            i++;
            token = strtok(NULL, " ");
        }
        // record the end time
        char endTime[3];
        end = start + duration;
        sprintf(endTime, "%d", end);
        strcpy(meeting_data[idx][4], endTime);        
        idx++;

    }
    //free(token);
    fclose(fp);

    // strip
    for (i = 0; i<idx-1; i++){
        meeting_data[i][3][strlen(meeting_data[i][3])-1] = '\0';
    }

    
    /**
     * sort based on the date
     */
    char temp[1024]; int day=0;
    for (i = 0; i<idx; i++){
        for (j = i+1; j<idx; j++){
            if (strcmp(meeting_data[i][0], meeting_data[j][0]) > 0){
                // then swap
                for (int k = 0; k<5; k++){
                    strcpy(temp, meeting_data[i][k]);
                    strcpy(meeting_data[i][k], meeting_data[j][k]);
                    strcpy(meeting_data[j][k], temp);
                }
                day++;
            }           
        }
    }

    /**
     * sort based on the start time
     */
    memset(temp, 0, sizeof(temp));
    for (i = 0; i<idx; i++){
        for (j = i+1; j<idx; j++){
            // should be from same day
            if (strcmp(meeting_data[i][0], meeting_data[j][0]) == 0){
                if (strcmp(meeting_data[i][2], meeting_data[j][2]) > 0){
                    // then swap

                    for (int k = 0; k<5; k++){
                        strcpy(temp, meeting_data[i][k]);
                        strcpy(meeting_data[i][k], meeting_data[j][k]);
                        strcpy(meeting_data[j][k], temp);
                    }
                }
            }
        }
    }

    /**
     * sort based on duration?
     */
    memset(temp, 0, sizeof(temp));
    for (i = 0; i<idx; i++){
        for (j = i+1; j<idx; j++){
            // should be from same day
            if (strcmp(meeting_data[i][0], meeting_data[j][0]) == 0){
                if (strcmp(meeting_data[i][2], meeting_data[j][2]) == 0){
                    if (strcmp(meeting_data[i][3], meeting_data[j][3]) > 0){
                        // then swap

                        for (int k = 0; k<5; k++){
                            strcpy(temp, meeting_data[i][k]);
                            strcpy(meeting_data[i][k], meeting_data[j][k]);
                            strcpy(meeting_data[j][k], temp);
                        }    
                    }
                    
                }
            }
        }
    }

    /**
     * sort based on name
     */

    memset(temp, 0, sizeof(temp));
    for (i = 0; i<idx; i++){
        for (j = i+1; j<idx; j++){
            // should be from same day
            if (strcmp(meeting_data[i][0], meeting_data[j][0]) == 0){
                if (strcmp(meeting_data[i][2], meeting_data[j][2]) == 0){
                    if (strcmp(meeting_data[i][3], meeting_data[j][3]) == 0){
                         if (strcmp(meeting_data[i][1], meeting_data[j][1]) > 0){
                            // then swap
                            for (int k = 0; k<5; k++){
                                strcpy(temp, meeting_data[i][k]);
                                strcpy(meeting_data[i][k], meeting_data[j][k]);
                                strcpy(meeting_data[j][k], temp);
                            }    
                         }
                    }
                    
                }
            }
        }
    }

    // print
    for(i = 0; i<idx; i++){
        for (j = 0; j<5; j++){
            printf("%s ",  meeting_data[i][j]);
        }printf("\n");
    }

    char timeSlot[18][9] = {'0', };
    for (i = 0; i<18; i++){
        for (j = 0; j<9; j++){
            timeSlot[i][j] = '0';
            //printf("%c ", timeSlot[i][j]);
        }
        //printf("\n");
    }


    // run SJF
    int pos = 0; int endTime = 0; int begin = 0;
    begin = atoi(meeting_data[0][2]);
    for (i = 0; i<idx; i++){
        if (i != idx -1){
            if (strcmp(meeting_data[i-1][0], meeting_data[i][0]) != 0){
                printf("\n\nnew day!\n");
                endTime = 0;
                pos++;
                printf("pos %d\n", pos);
            }
        }

        int startTime = atoi(meeting_data[i][2]);
        printf("start time %d endtime %d\n", startTime, endTime);
        if (endTime > startTime){
            printf("\trejected\n");
        }
        else{
            
            printf("%d \n", atoi(meeting_data[i][3]));
            for (j= 0; j < atoi(meeting_data[i][3]); j++){
                printf("recorded %s on %d\n", meeting_data[i][1], startTime+j);
                
                if (timeSlot[pos][startTime-9+j] != '0') break;
                //printf("%c \n", meeting_data[i][1][5]);
                printf("slot %d\n", startTime-9+j);
                timeSlot[pos][startTime-9+j] = meeting_data[i][1][5];
            }
            endTime = atoi(meeting_data[i][4]);
            printf("meeting finished at %d\n", endTime);
        }
        
    }

    for (i = 0; i<18; i++){
        for (j = 0; j<9; j++){
            printf("%c ", timeSlot[i][j]);
        }
        printf("\n");
    }



    /**
     * 
     * 
     * 
     * 
     * 
     * 
     */

    return;
}

int main(){
    int fd[13][2];
    SJF(fd);

    return 0;
}