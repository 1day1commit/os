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

struct Staff {
    int Project[5];
    int Manager;
    int Team[5];
    int attend_team_number;
};


struct Project {
    int Member[4];
    int Staff_number;
};


char create_pro_team[3] = {'c', 'p', 0};
char project_meeting_book[3] = {'p', 'b', 0};
char meeting_attend[3] = {'m', 'a', 0};
char fcfs[3] = {'f', 'f', 0};
char sjf[3] = {'s', 'j', 0};
char exit_PMS[3] = {'e', 'x', 0};


int extract_int(char *s, int start, int len);
int year(int yr);
int month(int yr, int mth);
bool is_valid_day(char *s1);
bool is_valid_time(char *s1);
int day(char *s1, char *s2);
char *IntToString(int num, char *str);
char *IntToDay(char *startDate, int start, int num);
char **split(char *str, char *delimiter);


void create_project_team(int fd[13][2], char *command, int len);
void test_cp_func(struct Staff *stuff, struct Project *project);
int single_input_meeting_request(int fd[13][2], char *useful_inf);
int batch_input_meeting_request(int fd[13][2], char *useful_inf);
void meeting_attendance_request(int fd[13][2]);
void FCFS(int fd[13][2], char useful_inf[30]);
void SJF(int fd[13][2]);
void print_schedule(char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], char *algorithm);
void analyse_attendance(int fd[13][2]);


char staffName[8][15] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};



int main() {
    FILE *input_file;
    input_file = fopen("Input_Meeting.txt", "w");
    fclose(input_file);
    int i,j;
    int toChild[13][2];
    int toParent[13][2];
    int pid = 0;
    int index = 0;
    struct Staff staff[8];
    struct Project project[5];
    
    for (i = 0; i < CHILD_NUMBER; i++) {
        staff[i].attend_team_number = 0;
        staff[i].Manager = -1;
        for (j = 0; j < 5; j++) {
            staff[i].Project[j] = -1;
        }
    }
    
    for (i = 0; i < CHILD_NUMBER; i++) {
        project[i].Staff_number = 0;
        for (j = 0; j < 4; j++) {
            project[i].Member[j] = -1;
        }
    }
    

    for (i = 0; i < CHILD_NUMBER; i++) {
        if (pipe(toChild[i]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
    }

    for (i = 0; i < CHILD_NUMBER; i++) {
        if (pipe(toParent[i]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
    }


    for (i = 0; i < CHILD_NUMBER; i++) {
        pid = fork();
        if (pid == 0 || pid < 0) {
            if (pid == 0) {
                index = i;
                break;
            }
        }
    }

    if (pid < 0) {
        printf("Fork Failed\n");
        exit(1);
    }

    else if (pid == 0) {
        for (i = 0; i < CHILD_NUMBER; i++) {
            if (i == index) {
                close(toParent[i][0]);
                close(toChild[i][1]);
            } else {     //maybe unnecessary
                close(toParent[i][0]);
                close(toParent[i][1]);
                close(toChild[i][0]);
                close(toChild[i][1]);
            }
        }
        
        char buf[100];
        int num;

        char operation[3];
        char information[8];

        while (true) {
            if ((num = read(toChild[index][0], buf, 10)) > 0) {
                operation[0] = buf[0];
                operation[1] = buf[1];
                operation[2] = 0;
                strncpy(information, buf + 2, 6);
                printf("%d Information: %s\n", index, information);
                if (index < 8) {// Staff
                    if (strcmp(operation, create_pro_team) == 0) {
                        printf("The information length %lu\n", strlen(information));
                        if (strlen(information) <= 3) {// member
                            printf("Add member\n");
                            staff[index].Project[staff[index].attend_team_number] = information[1] - 'A';
                            staff[index].Team[staff[index].attend_team_number] = information[0] - 'A';
                            staff[index].attend_team_number++;
                        }
                        if (strlen(information) > 3) {// manager
                            printf("Add manager\n");
                            staff[index].Project[staff[index].attend_team_number] = information[1] - 'A';
                            staff[index].Team[staff[index].attend_team_number] = information[0] - 'A';
                            staff[index].attend_team_number++;
                            staff[index].Manager = information[0] - 'A';
                        }
                    }
                    if (strcmp(operation, exit_PMS) == 0) {
                        break;
                    }
                } else {// Projects-Teams
                    if (strcmp(operation, create_pro_team) == 0) {
                        int staff_number = strlen(information);
                        i = 0;
                        while (staff_number > 0) {
                            project[index].Member[project[index].Staff_number] = information[i] - 'A';
                            project[index].Staff_number++;
                            staff_number--;
                            i++;
                        }
                    }
                    if (strcmp(operation, exit_PMS) == 0) {
                        break;
                    }
                }
            }
        }
        //close(toChild[index][0]);
        close(toParent[index][1]);
    }


    if (pid > 0) {
        char buf[100];
        char option[2];
        char command[100];
        int len;
        while (true) {
            printf("    ~~ WELCOME TO PolyStar ~~\n\n");
            printf("1.   Create Project Team\n\n");
            printf("2.   Project Meeting Request\n");
            printf("2a.  Single input\n2b.  Batch input\n2c.  Meeting Attendance\n\n");
            printf("3.  Print Meeting Schedule\n3a.  FCFS (First Come First Served)\n3b.  XXXX (Another algorithm implemented)\n3c.  YYYY (Attendance Report) \n\n");
            printf("4.  Exit\n\nEnter an option: ");
            fgets(command, 100, stdin);
            strncpy(option, command, 1);
            if (strncmp(option, "1", 1) == 0) {
                while (true) {
                    printf("\nEnter> ");
                    fgets(command, 100, stdin);
                    len = strlen(command);
                    command[--len] = 0;
                    if (strncmp(command, "0", 1) == 0) {
                        test_cp_func(staff, project);
                        printf("\n\n\n");
                        break;
                        
                    } else {
                        create_project_team(toChild, command, len);
                    }
                }

            } else if (strncmp(option, "2", 1) == 0) {
                while (true) {
                    printf("\nEnter> ");
                    fgets(command, 100, stdin);
                    len = strlen(command);
                    command[--len] = 0;    //**************************//
                    if (strncmp(command, "For 2a", 6) == 0) {
                        char useful_inf[30];
                        strncpy(useful_inf, command + 8, 30);
                        if (single_input_meeting_request(toChild, useful_inf) < 0) {
                            printf(">>>>>>Invalid time\n");
                        } else {
                            printf(">>>>>>Record\n");
                        }
                    } else if (strncmp(command, "For 2b", 6) == 0) {
                        char useful_inf[30];
                        strncpy(useful_inf, command + 8, 30);
                        int record_num = 0;
                        record_num = batch_input_meeting_request(toChild, useful_inf);
                        printf(">>>>>>%d meeting requests have been recorded\n", record_num);
                    } else if (strncmp(command, "0", 1) == 0) {
                        break;
                    } else {
                        meeting_attendance_request(toChild);
                    }
                }
            } else if (strncmp(option, "3", 1) == 0) {
                char useful_inf[30];
                while (true) {
                    fgets(command, 100, stdin);
                    if (strncmp(command, "For 3a", 6) == 0) {
                        strncpy(useful_inf, command + 13, 30);
                        FCFS(toChild, useful_inf);
                    } else if (strncmp(command, "For 3b", 6) == 0) {
                        SJF(toChild);
                    } else {
                        analyse_attendance(toChild);
                    }
                }
            } else if (strncmp(option, "4", 1) == 0) {
                for (i = 0; i < 13; i++) {
                    char temp[5];
                    strcpy(temp, exit_PMS);
                    write(toChild[i][1], temp, strlen(temp));    //need to add the null character
                }
                break;
            } else {
                printf("\nThe comment is illegal. Please re-enter the comment.\n");
            }
        }
        for(i=0; i<CHILD_NUMBER; i++){
            close(toChild[i][1]);
            //close(toChild[i][0]);
            close(toParent[i][0]);
            close(toParent[i][1]);
        }
        for (i = 1; i <= CHILD_NUMBER; i++) {
            wait(NULL);
        }
        exit(0);
    }
    return 0;
}


int readPipeManager(int fd[13][2], int managerIndex){
    char buffer[1024];
    printf("mi %d\n", managerIndex);
    read(fd[managerIndex][0], buffer, sizeof(buffer));
    printf("buffer %s\n", buffer);        

    return 0;
}



void create_project_team(int fd[13][2], char *command, int len) {
    char useful_inf[6];
    char **res = split(command, " ");
    int i = 0;
    while (res[i] != NULL) {
        if (i == 0) {
            useful_inf[i] = res[i][5];
        } else if (i == 1) {
            useful_inf[i] = res[i][8];
        } else {
            useful_inf[i] = res[i][0];
        }
        i++;
    }
    int useful_inf_len = i;
    char to_member_message[10];
    char to_manager_message[10];
    char to_project_message[10];
    char temp[5];
    char manager_message[3] = "MM";
    strcpy(to_member_message, create_pro_team);
    strcpy(to_manager_message, create_pro_team);
    strcpy(to_project_message, create_pro_team);
    strncpy(temp, useful_inf, 2);
    strcat(to_member_message, temp);
    strcat(to_manager_message, temp);
    strcat(to_manager_message, manager_message);
    strncpy(temp, useful_inf + 2, i - 2);
    strcat(to_project_message, temp);

        // team_a project_b cathy dee eff fds
        // useful info abcdef"
        // to manager cpabMM
        // member message cpab
        // project message cpcdef

    printf("useful info %s\n", useful_inf); // Team, Project, Manager, Member (initial)
    printf("to manager %s\n", to_manager_message); // cp, team, project, MM
    printf("member message %s\n", to_member_message); // cp, team, project
    printf("project message %s\n", to_project_message); // cp, manager initial, member, member, member



    for (i = 2; i < useful_inf_len; i++) {
        if (i == 2) {
            char buf[1024];
            printf("writing pipe manager %d %d\n", i, useful_inf[i]-'A');
            printf("useful_inf 2 %c\n", useful_inf[2]);
            write(fd[useful_inf[i] - 'A'][1], to_manager_message, strlen(to_manager_message));    //need to add the null character
            //read(fd[useful_inf[i] - 'A'][0], buf, sizeof(buf));
            printf("\n----------readPipe---------\n");

            readPipeManager(fd, useful_inf[2]-'A');            
            
        } else {
            printf("writing pipe member %d %d\n", i, useful_inf[i]-'A');
            write(fd[useful_inf[i] - 'A'][1], to_member_message, strlen(to_member_message));     //need to add the null character
        }
    }
    printf("writing pipe project %d\n", useful_inf[1]-'A'+8);
    write(fd[useful_inf[1] - 'A' + 8][1], to_project_message, strlen(to_project_message));
    sleep(1);
    printf("\n>>>>>> Project Team %c is created.\n", res[0][5]);


}

int single_input_meeting_request(int fd[13][2], char useful_inf[30]) {
    FILE *input_file;
    input_file = fopen("Input_Meeting.txt", "a");
    char team[2], day[11], start_time[6], hours[2];
    strncpy(team, useful_inf + 5, 1);
    strncpy(day, useful_inf + 7, 10);
    strncpy(start_time, useful_inf + 18, 5);
    strncpy(hours, useful_inf + 24, 1);
    if (!is_valid_day(day)) {
        printf("It is invalid time\n");
        return -1;
    }
    if (!is_valid_time(start_time)) {
        printf("It is invalid time\n");
        return -1;
    }
    fprintf(input_file, "%s\n", useful_inf);
    fclose(input_file);
    return 1;
}


int batch_input_meeting_request(int fd[13][2], char *command) {
    printf("command : %s\n", command);
    char useful_inf[30];
    char toInputFile[30];
    strcpy(useful_inf, command);
    strcpy(toInputFile, "For 2b, ");
    int i = 0, line_num = 0, len = 0;
    char buf[MAX_LINE];
    char use_inf[30];
    FILE *fp;
    printf("%s\n", useful_inf);
    if ((fp = fopen(useful_inf, "r")) == NULL) {
        printf("fail to read the file\n");
        return -1;
    }

    while (fgets(buf, MAX_LINE, fp) != NULL) {
        len = strlen(buf);
        buf[len - 1] = '\0';    //***********************//
        strcpy(use_inf, buf);
        strcat(toInputFile, use_inf);
        if (single_input_meeting_request(fd, toInputFile) > 0) {
            line_num++;
        }
    }
    fclose(fp);
    return line_num;
}

void meeting_attendance_request(int fd[13][2]) {
    return;
}




void FCFS(int fd[13][2], char useful_inf[30]) {
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

    /**
     * sort based on the date
     */
    char temp[1024];
    for (i = 0; i<index1-1; i++){    //for set_meetings
        for (j = i+1; j<index1; j++){
            if (strcmp(set_meetings[i][0], set_meetings[j][0]) > 0){
                // then swap
                for (int k = 0; k<5; k++){
                    strcpy(temp, set_meetings[i][k]);
                    strcpy(set_meetings[i][k], set_meetings[j][k]);
                    strcpy(set_meetings[j][k], temp);
                }
            }           
        }
    }

    memset(temp, 0, sizeof(temp));
    for (i = 0; i<index2-1; i++){     //for rejected_meetings
        for (j = i+1; j<index2; j++){
            if (strcmp(rejected_meetings[i][0], rejected_meetings[j][0]) > 0){
                // then swap
                for (int k=0; k<5; k++){
                    strcpy(temp, rejected_meetings[i][k]);
                    strcpy(rejected_meetings[i][k], rejected_meetings[j][k]);
                    strcpy(rejected_meetings[j][k], temp);
                }
            }           
        }
    }


    /**
     * sort based on the start time
     */
    memset(temp, 0, sizeof(temp));
    for (i = 0; i<index1-1; i++){
        for (j = i+1; j<index1; j++){
            // should be from same day
            if (strcmp(set_meetings[i][0], set_meetings[j][0]) == 0){
                if (atoi(set_meetings[i][2]) > atoi(set_meetings[j][2])){
                    // then swap
                    for (int k = 0; k<5; k++){
                        strcpy(temp, set_meetings[i][k]);
                        strcpy(set_meetings[i][k], set_meetings[j][k]);
                        strcpy(set_meetings[j][k], temp);
                    }
                }
            }
        }
    }

    memset(temp, 0, sizeof(temp));
    for (i = 0; i<index2-1; i++){
        for (j = i+1; j<index2; j++){
            // should be from same day
            if (strcmp(rejected_meetings[i][0], rejected_meetings[j][0]) == 0){
                if (atoi(rejected_meetings[i][2]) > atoi(rejected_meetings[j][2])){
                    // then swap
                    for (int k = 0; k<5; k++){
                        strcpy(temp, rejected_meetings[i][k]);
                        strcpy(rejected_meetings[i][k], rejected_meetings[j][k]);
                        strcpy(rejected_meetings[j][k], temp);
                    }
                }
            }
        }
    }

    /* FOR THE OUTPUT FUNCTION: 
    *  So "set_meetings" contains all the scheduled meetings and index1 is the number of accepted meetings
    * "rejected_meetings" contains all the rejected meetings and index2 is the number of rejected meetings
    *  useful_inf will give start date for fcfs and useful_inf + 11 will give end date for fcfs
    *  Need to call the output function from inside of this function with all four of these parameters 
    *  A fifth parameter might be needed to specify which function is calling the output function, i.e. fcfs or sjf
    *  For printing meeting schedule of each staff, at first we need to read the data from the pipe of each of the child process (1-8)
    *  This is to get info from the Staff struct of each of the child, so that we know which staff is involved in which projects 
    */

    return;
}




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





    return;
}


void analyse_attendance(int fd[13][2]) {
    return;
}


int extract_int(char *s, int start, int len) {
    char ss[5];
    strncpy(ss, s + start, len);
    ss[len] = 0;
    return atoi(ss);
}


int year(int yr) {
    if ((yr % 400 == 0) || (yr % 100 != 0 && yr % 4 == 0))
        return 1;
    else
        return 0;
}


int month(int yr, int mth) {
    int days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mth != 2)
        return days[mth];
    else
        return days[mth] + year(yr);
}


bool is_valid_day(char *s1) {
    int yr, mth, dy;
    int mth_and_day;
    yr = extract_int(s1, 0, 4);
    mth = extract_int(s1, 5, 2);
    dy = extract_int(s1, 8, 2);
    if (yr != 2022) {
        return false;
    }
    mth_and_day = mth * 100 + dy;
    if (mth_and_day < 415 || mth_and_day > 514) {
        return false;
    } else {
        if (mth_and_day == 417 || mth_and_day == 424 || mth_and_day == 501 || mth_and_day == 508 || mth_and_day == 515) {
            return false;
        }
    }
    return true;
}


bool is_valid_time(char *s1) {
    int time;
    time = extract_int(s1, 0, 2);
    if (time < 9 || time > 18) {
        return false;
    }
    return true;
}


int day(char *s1, char *s2) {

    int yr1, mth1, dy1;
    int yr2, mth2, dy2;
    int d_yr = 0, d_mth = 0, d_dy = 0;

    yr1 = extract_int(s1, 0, 4);
    mth1 = extract_int(s1, 5, 2);
    dy1 = extract_int(s1, 8, 2);

    yr2 = extract_int(s2, 0, 4);
    mth2 = extract_int(s2, 5, 2);
    dy2 = extract_int(s2, 8, 2);

    if ((yr1 > yr2) || (yr1 == yr2 && mth1 > mth2) || (yr1 == yr2 && mth1 == mth2 && dy1 > dy2))
        return -1;

    if (mth1 > 12 || mth2 > 12 || month(yr1, mth1) < dy1 || month(yr2, mth2) < dy2)
        return -1;

    int i;

    for (i = yr1 + 1; i <= yr2 - 1; i++) {
        d_yr = d_yr + 365 + year(i);
    }

    if (yr1 == yr2) {
        for (i = mth1; i < mth2; i++)
            d_mth = d_mth + month(yr1, i);
    } else {
        for (i = mth1; i <= 12; i++) {
            d_mth = d_mth + month(yr1, i);
        }
        for (i = 1; i < mth2; i++) {
            d_mth = d_mth + month(yr2, i);
        }
    }

    if (yr1 == yr2 && mth1 == mth2) {
        d_dy = dy2 - dy1 + 1;
    } else {
        d_dy = -dy1 + 1 + dy2;
    }

    return d_yr + d_mth + d_dy;
}


char *IntToString(int num, char *str) {
    int length = 0;
    do {
        str[length] = num % 10 + 48;
        length++;
        num /= 10;
    } while (num != 0);

    int j = 0;
    str[length] = '\0';
    for (; j < length / 2; j++) {
        char temp;
        temp = str[j];
        str[j] = str[length - 1 - j];
        str[length - 1 - j] = temp;
    }
    return str;
}


char *IntToDay(char *startDate, int start, int num) {
    char FIRST_DAY[11] = "xxxx-01-01";
    char year[4];
    int YEAR, days = 365, i = 0, thisDate, j;
    int month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char mon[12][3] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    char day[31][3] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    for (j = 0; j < 4; j++) {
        FIRST_DAY[j] = startDate[j];
        year[j] = startDate[j];
    }
    YEAR = atoi(year);
    if (start == -1) printf("The date is illegal. Please reenter the comment.\n");
    else {
        thisDate = start + num;
        if (YEAR % 4 == 0) {
            days += 1;
            month[1] = 29;
        }
        if (start + num <= days) {
            i = 0;
            while (thisDate > month[i]) {
                thisDate -= month[i];
                i++;
            }
            for (j = 0; j < 2; j++) startDate[j + 5] = mon[i][j];
            for (j = 0; j < 2; j++) startDate[j + 8] = day[thisDate - 1][j];
        } else {
            printf("The date is illegal. Please reenter the comment.\n");
        }
    }
    return startDate;
}

char **split(char *str, char *delimiter) {
    int len = strlen(str);
    char *strCopy = (char *) malloc((len + 1) * sizeof(char));
    strcpy(strCopy, str);
    int i, j;
    for (i = 0; strCopy[i] != '\0'; i++) {
        for (j = 0; delimiter[j] != '\0'; j++) {
            if (strCopy[i] == delimiter[j]) {
                strCopy[i] = '\0';
                break;
            }
        }
    }
    char **res = (char **) malloc((len + 2) * sizeof(char *));
    len++;
    int resI = 0;
    for (i = 0; i < len; i++) {
        res[resI++] = strCopy + i;
        while (strCopy[i] != '\0') {
            i++;
        }
    }
    res[resI] = NULL;
    return res;
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


void test_cp_func(struct Staff *staff, struct Project *project) {
    int i, k;
    sleep(2);
    printf("\n\n\n");
    printf("STAFF INFO\n----------\n");
    for (i = 0; i < 8; ++i) {
        printf("Staff %d %s is manager of %d\n", i, staffName[i], staff[i].Manager);
        for (k = 0; k < staff[i].attend_team_number; ++k) {
            printf("Works on the project: %d\n", staff[i].Project[k]);
            printf("Works on the team: %d\n", staff[i].Team[k]);
        }
        printf("***************\n");
    }

    printf("\n\n\n");

    printf("PROJECT INFO\n------------\n");
    for (i = 0; i < 5; ++i) {
        printf("Project %d\n", i);
        for (k = 0; k < project[i].Staff_number; ++k) {
            printf("%d ", project[i].Member[k]);
        }
        printf("***************\n");
    }
}
