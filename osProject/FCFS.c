//Just for testing Scheduling and Output
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define CHILD_NUMBER 13
#define MAX_LINE 162
#define MAX_REQUEST 999
void print_schedule(char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], char *algorithm);

void FCFS() {
    int i, j, k, l;
    char *algorithm = "FCFS";
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
    for (i = 0; i<idx-1; i++){   /*************** MAYBE NOT -1 *************/
        meeting_data[i][3][strlen(meeting_data[i][3])-1] = '\0';    /*************** MAYBE NOT -1 *************/
    }

    char set_meetings[162][5][1024];    //used for keeping all the accepted meetings
    char rejected_meetings[162][5][1024];   //used for keeping all the rejected meetings

    for(i=0; i<idx; i++){
        for(j=0; j<5; j++){
            strcpy(set_meetings[i][j], "0");    //using "0" as placeholders
            strcpy(rejected_meetings[i][j], "0");
        }
    }

    char start_date[12];
    char end_date[12];
    int len, index1=0, index2=0;    //index1 gives the no of accepted meeting requests and index2 gives the no of rejected meeting requests
    strcpy(start_date, "2022-04-25");  //record the starting date
    strcpy(end_date, "2022-05-14");   //record the end date
    len = strlen(start_date);   
    start_date[len] = '\0';    
    len = strlen(end_date);
    end_date[len] = '\0';

    // strncpy(start_date, useful_inf, 10); 
    // strncpy(end_date, useful_inf + 11, 10);   

    for(i=0; i<idx; i++){
        for(j=0; j<5; j++){
            if(j == 0){
                if(strcmp(meeting_data[i][j], start_date) < 0 || strcmp(meeting_data[i][j], end_date) > 0){   //if date of meeting is outside the allowed meeting schedule
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }
            }
            else if(j == 2){
                if(atoi(meeting_data[i][i]) < 9){    //if starting time of meeting less than 09:00
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }
            }
            else if(j == 4){     //if ending time of meeting more than 18:00
                if(atoi(meeting_data[i][i]) > 18){
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }                
            }
            for(k=0; k<162; k++){   ///MAYBE need to change 162
                //to check for new meetings with the already scheduled meetings
                if(strcmp(set_meetings[k][0], meeting_data[i][0]) == 0){     //if meeting on the same day
                
                    //if the meeting does not conflict with the time period of an already scheduled meeting
                    if((strcmp(meeting_data[i][2], set_meetings[k][2]) <= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][2]) <= 0) || (strcmp(meeting_data[i][2], set_meetings[k][4]) >= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][4]) >= 0)){
                        for(l=0; l<5; l++){
                            strcpy(set_meetings[index1][l], meeting_data[i][l]);
                        }
                        index1++;
                    } //if meeting conflicts
                    else{
                        for(l=0; l<5; l++){
                            strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                        }
                        index2++;                        
                    }
                }
            }

        }
    }
    for (int x=0; x<index2; x++){
        for(int y=0; y<5;y++){
            printf("rejected: %s\n",rejected_meetings[x][y]);
        }
    }


    print_schedule(set_meetings, rejected_meetings, algorithm);
}

void print_schedule(char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], char *algorithm) {
     // Need a struct or array that have total, rejected, accepted meeting requests
    // ================== something like this ==================
    // struct Total_MeetingRequests{
    //     char total_team[MAX_REQUEST][10]; //Team_A, Team_B
    //     char total_date[MAX_REQUEST][20]; // 2022-04-25
    //     char total_startTime[MAX_REQUEST][20]; // 09:00
    //     int total_hours[MAX_REQUEST]; // 2(hours)
    // } total_MeetingRequests;
    // struct Accepted_MeetingRequests{
    //     char accepted_team[MAX_REQUEST][10]; 
    //     char accepted_date[MAX_REQUEST][20]; 
    //     char accepted_startTime[MAX_REQUEST][20]; 
    //     int accepted_hours[MAX_REQUEST];
    // } accepted_MeetingRequests;
    // struct Rejected_MeetingRequests{
    //     char rejected_team[MAX_REQUEST][10]; 
    //     char rejected_date[MAX_REQUEST][20]; 
    //     char rejected_startTime[MAX_REQUEST][20]; 
    //     int rejected_hours[MAX_REQUEST]; 
    // } rejected_MeetingRequests;
    //============================================================
    
    int i, j;
    int p = 0, k = 0, accepted_length = 0, rejected_length = 0; //number of scheduled and rejected requests
    while(1){ //getting the number of accepted meetings
        if (strcmp(accepted_meetings[p][0],"") != 0 && strcmp(accepted_meetings[p][0],"0") != 0){
            p++;
            accepted_length ++;
        } else {
            break;
        }
    }
    while(1){ //getting the number of rejected requests
        if (strcmp(rejected_meetings[k][0],"") != 0 && strcmp(rejected_meetings[p][0],"0") != 0){
            k++;
            rejected_length ++;
        } else {
            break;
        }
    }
    // for (int x=0; x<accepted_length; x++){
    //     printf("Working loop?\n");
    //     for(int y=0; y<5;y++){
    //         printf("accepted: %s\n",accepted_meetings[x][y]);
    //     }
    // }

    int total_length = accepted_length + rejected_length;
    printf("Accepted length: %d, Rejected length: %d \n", accepted_length, rejected_length);
    //===========FOR TESTING ONLY===========
    char *algorithmA = "FCFS";
    char *startDate = "2022-04-25";
    char *endDate = "2022-04-27";
    char *startTime = "09:00";
    char *teamName = "Team_A";
    char *projectName = "Project A";
    char staffName[8][10] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};
    char *blank = "";
    //===========FOR TESTING ONLY===========
    FILE *fp;
    char filename[30];
    int num = 1; //file name
    int result = sprintf(filename, "Schedule_%s_%02d.txt",algorithm, num);
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
        // char startTime[] = {rejected_meetings[i][2][0], rejected_meetings[i][2][1], '\0' }; // get starting time to calculate End time
        // int endTime = atoi(startTime) + atoi(rejected_meetings[i][3]); //calculate endtime
        fprintf(fp, "%s %11s %5s:00 %10s       Project_%c\n\n",accepted_meetings[i][j], accepted_meetings[i][j+2], accepted_meetings[i][j+4], accepted_meetings[i][j+1], accepted_meetings[i][j+1][5]); //need to work with projects
    }
    for (i = 0; i < 8; i++) {
        fprintf(fp, "====================================================================================== \n");
        fprintf(fp, "Staff: %s \n\n", staffName[i]);
        fprintf(fp, "Date             Start    End       Team         Project           \n");
        fprintf(fp, "====================================================================================== \n");
        fprintf(fp, "%s %11s %8s %10s %15s\n\n", startDate, startTime, startTime, teamName, projectName);

        fprintf(fp, "====================================================================================== \n");
    }
    fprintf(fp, "%36s - End - %36s\n", blank, blank);

    fprintf(fp, "Meeting Request - REJECTED *** \n\n");
    fprintf(fp, "There are %d requests rejected for the required period.\n\n", rejected_length);
    fprintf(fp, "====================================================================================== \n");

    for( i = 0; i < rejected_length; i++){
        j=0;
        fprintf(fp, "%d.  %s  %s  %s  %s\n", i+1, rejected_meetings[i][j+1], rejected_meetings[i][j], rejected_meetings[i][j+2], rejected_meetings[i][j+3]);
    } 

    fprintf(fp, "====================================================================================== \n");

    int accepted_hours = 0;
    int rejected_hours = 0;
    for (i = 0; i < accepted_length; i++){
        accepted_hours += atoi(accepted_meetings[i][3]);
    }
    printf("accepted hours: %d\n", accepted_hours);
    for (i = 0; i < rejected_length; i++){
        rejected_hours += atoi(rejected_meetings[i][3]);
    }
    printf("rejected hours: %d\n", rejected_hours);
    int total_hours = accepted_hours + rejected_hours;
    printf("total hours: %d\n", total_hours);

    float total_utilization = (float)total_hours / (float)162; 
    printf("total utilization: %.2f\n", total_utilization);
    float accepted_utilization = (float)accepted_hours / (float)162;
    float rejected_utilization = (float)rejected_hours / (float)162;
    char *blanks = "";

    fprintf(fp, "Performance: \n\n");
    fprintf(fp, "Total Number of Requests Received: %d (%.2f%%)\n", total_length, total_utilization);
    fprintf(fp, "%5s Number of Requests Received: %d (%.2f%%)\n", blanks, accepted_length, accepted_utilization);
    fprintf(fp, "%5s Number of Requests Rejected: %d (%.2f%%)\n\n", blanks, rejected_length, rejected_utilization);
    fprintf(fp, "Utilization of Time Slot: \n\n");
    fprintf(fp, "%4s Accepted request %10s - %.1f%%\n", blanks, blanks, total_utilization);
    //===================== needs to receive Team & staff info ===============
    fprintf(fp, "%4s Team_A %20s - %.1f%%\n", blanks, blanks, accepted_utilization);
    fprintf(fp, "%4s Team_B %20s - %.1f%%\n", blanks, blanks, accepted_utilization);
    fprintf(fp, "%4s Staff_A %19s - %.1f%%\n", blanks, blanks, accepted_utilization);
    fprintf(fp, "%4s Staff_B %19s - %.1f%%\n", blanks, blanks, accepted_utilization);

    fclose(fp);
}


int main(int argc, char const *argv[])
{
    FCFS();
    return 0;
}

 