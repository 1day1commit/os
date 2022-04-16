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

void SJF(int fd[13][2], int read_data[8][5]);
void print_schedule(int fd[13][2], int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm);


void SJF(int fd[13][2], int read_data[8][5]) {
    int i, j, k, l; 
    int rejectedMeetingCount = 0;
    int approvedMeetingCount = 0;
    // 1. read input file
    FILE *fp;
    fp = fopen("Input_Meeting.txt", "r");

    // 2. read project info
    // Team_A 2022-04-25 09:00 2
    // 3. store data
    char meeting_data[162][5][1024];
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

    char set_meetings[162][5][1024];
    char rejected_meetings[162][5][1024];


    for(i=0; i<162; i++){
        for(j=0; j<5; j++){
            strcpy(set_meetings[i][j], "0");    //using "0" as placeholders
            strcpy(rejected_meetings[i][j], "0");
        }
    }

    char start_date[11];
    char end_date[11];
    int len, index1=0, index2=0, track;    //index1 gives the no of accepted meeting requests and index2 gives the no of rejected meeting requests
    strcpy(start_date, "2022-04-25");  //record the starting date
    strcpy(end_date, "2022-05-14");   //record the end date
    //len = strlen(start_date);   
    //start_date[len-1] = '\0';    
    //len = strlen(end_date);
    //end_date[len-1] = '\0';

    //지우지 마셈!!
    for(i=0; i<idx; i++){
        track = 1;
        printf("---> ");
        printf("%s %s %s %s %s\n", 
        meeting_data[i][0], meeting_data[i][1], meeting_data[i][2], meeting_data[i][3], meeting_data[i][4]);
        for(j=0; j<5; j++){
            if(j == 0){ // comparing date (2022-04-25)
                if(strcmp(meeting_data[i][j], start_date) < 0){   //if date of meeting is outside the allowed meeting schedule
                    track = 0;
                    printf("Rejected from date ");
                    for(l=0; l<5; l++){
                        printf("%s ", meeting_data[i][l]);
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    printf("\n");
                    rejectedMeetingCount++;
                    break;
                }
            }
            else if(j == 2){ // comparing time (09:00)
                if(atoi(meeting_data[i][j]) < 9){    //if starting time of meeting less than 09:00
                    track = 0;
                    printf("Rejected from time ");
                    for(l=0; l<5; l++){
                        printf("%s ", meeting_data[i][l]);
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    printf("\n");
                    rejectedMeetingCount++;
                    break;
                }
            }
            else if(j == 4){ //if ending time of meeting more than 18:00 
                if(atoi(meeting_data[i][j]) > 18){
                    track = 0;
                    printf("Rejected from 미팅 시간 아웃 ");
                    for(l=0; l<5; l++){
                        printf("%s ", meeting_data[i][l]);
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    printf("\n");
                    rejectedMeetingCount++;
                    break;
                }  
                for(k=0; k<162; k++){   ///MAYBE need to change 162
                    //to check for new meetings with the already scheduled meetings
                    if(strcmp(set_meetings[k][0], meeting_data[i][0]) == 0){     //if meeting on the same day
                        track = 0;
                        //if the meeting does not conflict with the time period of an already scheduled meeting
                        if((strcmp(meeting_data[i][2], set_meetings[k][2]) <= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][2]) <= 0) || (strcmp(meeting_data[i][2], set_meetings[k][4]) >= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][4]) >= 0)){
                            for(l=0; l<5; l++){
                                strcpy(set_meetings[approvedMeetingCount][l], meeting_data[i][l]);
                            }
                            //approvedMeetingCount++;
                        } //if meeting conflicts
                        else{
                            for(l=0; l<5; l++){
                                strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                            }
                            rejectedMeetingCount++;
                            break;                        
                        }
                    }
                }              
            }
        }
        // if(track == 1){
        //     for(l=0; l<5; l++){
        //         strcpy(set_meetings[approvedMeetingCount][l], meeting_data[i][l]);
        //     }
        //     approvedMeetingCount++;          
        // }
    }
    

    
    /**
     * sort based on the date
     */
    char temp[1024]; int day=0;
    for (i = 0; i<idx; i++){
        for (j = i+1; j<idx; j++){
            if (strcmp(meeting_data[i][0], meeting_data[j][0]) > 0){
                // then swap
                for (k = 0; k<5; k++){
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

                    for (k = 0; k<5; k++){
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

                        for (k = 0; k<5; k++){
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
                            for (k = 0; k<5; k++){
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
        printf("\n\n\n\nMeeting---> ");
        printf("%s %s %s %s %s\n", 
            meeting_data[i][0], meeting_data[i][1], meeting_data[i][2], meeting_data[i][3], meeting_data[i][4]);
        // until it is not last meeting
        if (i != idx && i != 0){
            // check if the date are same
            // if not, reset the end time
            if (strcmp(meeting_data[i-1][0], meeting_data[i][0]) != 0){
                printf("\n\nnew day!\n");
                endTime = 0;
                pos++;
                printf("pos %d\n", pos);
            }
        }

        // extract start time
        int startTime = atoi(meeting_data[i][2]);
        printf("start time %d endtime %d\n", startTime, endTime);

        // if end time is bigger than start time, reject
        // e.g) previous meeting ends at 11:00AM and next meeting starts at 9:00AM
        if (endTime > startTime){
            printf("\trejected\n");
            rejectedMeetingCount++;
            printf("rejected Meeting %d\n", rejectedMeetingCount);

            // copy meeting info as rejected
            for (j = 0; j<5; j++){
                printf("meeting_data %s\n", meeting_data[i][j]);
                strcpy(rejected_meetings[rejectedMeetingCount-1][j], meeting_data[i][j]);

            }
        }

        // if not,
        else{ // approved meeting
            printf("approved meeting %d\n", approvedMeetingCount);
            // copy meeting info as approved
            for (k = 0; k<5; k++){
                printf("%s ", meeting_data[i][k]);
                strcpy(set_meetings[approvedMeetingCount][k], meeting_data[i][k]);
            }
            printf("\n\n");        
            printf("---> ");
            printf("%s %s %s %s %s\n", 
            set_meetings[approvedMeetingCount][0], set_meetings[approvedMeetingCount][1], set_meetings[approvedMeetingCount][2], set_meetings[approvedMeetingCount][3], set_meetings[approvedMeetingCount][4]);
            approvedMeetingCount++;

            // printf("%d \n", atoi(meeting_data[i][3]));
            // for (j= 0; j < atoi(meeting_data[i][3]); j++){ // duration

            //     printf("recorded %s on %d\n", meeting_data[i][1], startTime+j);
                
            //     if (timeSlot[pos][startTime-9+j] != '0') break;
            //     //printf("%c \n", meeting_data[i][1][5]);
            //     printf("slot %d\n", startTime-9+j);
            //     timeSlot[pos][startTime-9+j] = meeting_data[i][1][5];
                
            // }
            endTime = atoi(meeting_data[i][4]);
            printf("meeting finished at %d\n", endTime);
        }

        
    
        
    }

    printf("rejected Meeting %d\n", rejectedMeetingCount);
    approvedMeetingCount = idx - rejectedMeetingCount;
    printf("Approved Meeting %d\n", approvedMeetingCount); 

    for (i = 0; i<approvedMeetingCount; i++){
        for (j = 0; j<5; j++){
            printf("%s ", set_meetings[i][j]);
        }
        printf("\n");
    }


    

    print_schedule(fd, read_data, set_meetings, rejected_meetings, approvedMeetingCount, rejectedMeetingCount, "SJF");


    return;
}







void print_schedule(int fd[13][2], int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm) {    
    int i, j, k, l;
    int accepted_length, rejected_length; //number of scheduled and rejected requests

    accepted_length = index1;
    rejected_length = index2;

    int total_length = accepted_length + rejected_length;

    char staffName[8][10] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};
    char teamName[5][10] = {"Team_A", "Team_B", "Team_C", "Team_D", "Team_E"};
    char *blank = "";
    int staffHours[8] = {0};

    FILE *fp;
    char filename[30];

    int num = 1; //file name (MAYBE HAS TO BE A GLOBAL VARIABLE)

    int result = sprintf(filename, "Schedule_%s_%02d.txt",algorithm, num);

    int teams[5];

    num++;
    
    fp = fopen(filename,"w+");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open the file");
        // return 1;
    }
    fprintf(fp, "*** Project Meeting *** \n\n");
    fprintf(fp, "Algorithm used: %s \n", algorithm);
    // fprintf(fp, "Period: %s to %s \n\n", useful_inf, endDate); // where to get period?
    fprintf(fp, "Date             Start    End       Team         Project           \n");
    fprintf(fp, "====================================================================================== \n");
    for (i = 0; i < accepted_length; i++){
        j=0;
        fprintf(fp, "%s %11s %5s:00 %10s       Project_%c\n\n",accepted_meetings[i][j], accepted_meetings[i][j+2], accepted_meetings[i][j+4], accepted_meetings[i][j+1], accepted_meetings[i][j+1][5]); //need to work with projects
    }

    for (i = 0; i < 8; i++) {
        fprintf(fp, "====================================================================================== \n");
        fprintf(fp, "Staff: %s \n\n", staffName[i]);
        fprintf(fp, "Date             Start    End       Team         Project           \n");
        fprintf(fp, "====================================================================================== \n");

        for(l=0; l<accepted_length; l++){
            int num = accepted_meetings[l][1][5] - 'A';
            for(k=0; k<5; k++){
                if(read_data[i][k] == -1){
                    continue;
                }
                else if(read_data[i][k] == num){
                    j=0;
                    staffHours[i] += atoi(accepted_meetings[l][j+3]); // For calculation of each Staff's utilization
                    fprintf(fp, "%s %11s %5s:00 %10s       Project_%c\n\n",accepted_meetings[l][j], accepted_meetings[l][j+2], accepted_meetings[l][j+4], accepted_meetings[l][j+1], accepted_meetings[l][j+1][5]); //need to work with projects
                    break;
                }

            }

        }
    }
    fprintf(fp, "====================================================================================== \n");
    fprintf(fp, "%36s - End - %36s\n", blank, blank);

    fprintf(fp, "Meeting Request - REJECTED *** \n\n");
    fprintf(fp, "There are %d requests rejected for the required period.\n\n", rejected_length);
    fprintf(fp, "====================================================================================== \n");

    for( i = 0; i < rejected_length; i++){
        j=0;
        fprintf(fp, "%d.  %s  %s  %s  %s\n", i+1, rejected_meetings[i][j+1], rejected_meetings[i][j], rejected_meetings[i][j+2], rejected_meetings[i][j+3]);
    } 

    fprintf(fp, "====================================================================================== \n");

    int accepted_hours=0;
    int rejected_hours=0;
    for (i = 0; i < accepted_length; i++){
        accepted_hours += atoi(accepted_meetings[i][3]);
    }
    for (i = 0; i < rejected_length; i++){
        rejected_hours += atoi(rejected_meetings[i][3]);
    }


    float total_utilization = ((float)(accepted_hours + rejected_hours) / 162.0) * 100.0; 
    float accepted_utilization = ((float)accepted_hours / 162.0) * 100.0;
    float rejected_utilization = ((float)rejected_hours / 162.0) * 100.0;
    char *blanks = "";

    // printf("Accepted hours: %d, Rejected hours: %d\n", accepted_hours, rejected_hours);
    // printf("Total Utilization: %f, accepted_utilization: %f\n", total_utilization, accepted_utilization);

    fprintf(fp, "Performance: \n\n");
    fprintf(fp, "Total Number of Requests Received: %d (%.1f%%)\n", total_length, total_utilization);
    fprintf(fp, "%5s Number of Requests Accepted: %d (%.1f%%)\n", blanks, accepted_length, accepted_utilization);
    fprintf(fp, "%5s Number of Requests Rejected: %d (%.1f%%)\n\n", blanks, rejected_length, rejected_utilization);
    fprintf(fp, "Utilization of Time Slot: \n\n");
    fprintf(fp, "%4s Accepted request %10s - %.1f%%\n", blanks, blanks, accepted_utilization);
    //===================== needs to receive Team & staff info ===============

    rejected_hours = 0;
    for(i=0; i<5; i++){
        accepted_hours = 0;
        for(j=0; j<accepted_length; j++){
            if(strcmp(teamName[i], accepted_meetings[j][1]) == 0){
                accepted_hours += atoi(accepted_meetings[j][3]);
            }
        }
        accepted_utilization = ((float)accepted_hours / 162.0) * 100.0;
        fprintf(fp, "%4s %s %20s - %.1f%%\n", blanks, teamName[i], blanks, accepted_utilization);
    }
    for (i = 0; i < 8; i++){
        float staff_Utilization = ((float)staffHours[i] / 162.0) * 100;
        fprintf(fp, "%4s Staff_%c %19s - %.1f%%\n", blanks, staffName[i][0], blanks, staff_Utilization);
        
    }

    
    fclose(fp);
    printf("Printed. Export file name: %s\n", filename);

    return;
}


int main(){
    int fd[13][2]; int read_data[8][5];
    SJF(fd, read_data);

    return 0;
}