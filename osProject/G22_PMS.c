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
char pass_data[3] = {'p', 's', 0};
char find_managers[3] = {'f', 'm', 0};


int extract_int(char *s, int start, int len);
bool is_valid_time(char *s1);
bool is_valid_duration(int x);
char **split(char *str, char *delimiter);


void create_project_team(int fd[13][2], char *command, int len, int read_manager[8], int read_data[8][5]);
void test_cp_func(struct Staff *stuff, struct Project *project);
int single_input_meeting_request(int fd[13][2], char *useful_inf);
int batch_input_meeting_request(int fd[13][2], char *useful_inf);
void meeting_attendance_request(char start_date[11], char end_date[11], int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm);
void FCFS(char useful_inf[30], int read_data[8][5], char *command);
void SJF(char useful_inf[30], int read_data[8][5], char *command);
void print_schedule(int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, int time_peroid, char *algorithm);
void analyse_attendance(char useful_inf[30], char start_date[11], char end_date[11], int time_period, int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm);

// Project staff count
int proj_participation[8] = {0,0,0,0,0,0,0,0};
int is_team_created[5] = {0,0,0,0,0};
int is_project_created[5] = {0,0,0,0,0};
//Store Utilization from FCFS scheduling
float store_util[2] = {0.0};
int file_num = 1;



int main() {
    // FILE *input_file;
    // input_file = fopen("Input_Meeting.txt", "w");
    // fclose(input_file);
    int i,j;
    int toChild[13][2];
    int toParent[13][2];
    int pid = 0;
    int index = 0;
    struct Staff staff[8];
    struct Project project[5];
    
    for (i = 0; i < 8; i++) {
        staff[i].attend_team_number = 0;
        staff[i].Manager = -1;
        for (j = 0; j < 5; j++) {
            staff[i].Project[j] = -1;
            staff[i].Team[j] = -1;
        }
    }
    
    for (i = 0; i < 5; i++) {
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

                if(strcmp(operation, pass_data) == 0){
                    write(toParent[index][1], staff[index].Project, sizeof(staff[index].Project));
                    continue;
                }

                if(strcmp(operation, find_managers) == 0){
                    write(toParent[index][1], &staff[index].Manager, sizeof(staff[index].Manager));
                    continue;
                }

                if (index < 8) {// Staff
                    if (strcmp(operation, create_pro_team) == 0) {
                        //printf("The information length %lu\n", strlen(information));
                        if (strlen(information) <= 3) {// member
                            staff[index].Project[staff[index].attend_team_number] = information[1] - 'A';
                            staff[index].Team[staff[index].attend_team_number] = information[0] - 'A';
                            staff[index].attend_team_number++;
                        }
                        if (strlen(information) > 3) {// manager
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
        close(toChild[index][0]);
        close(toParent[index][1]);
    }


    if (pid > 0) {
        for (i = 0; i < CHILD_NUMBER; i++) {
            close(toParent[i][1]);
            close(toChild[i][0]);
        }
        char buf[100];
        char option[2];
        char command[100];
        int read_data[8][5];
        int len;
        while (true) {
            printf("    ~~ WELCOME TO PolyStar ~~\n\n");
            printf("1.   Create Project Team\n\n");
            printf("2.   Project Meeting Request\n");
            printf("2a.  Single input\n2b.  Batch input\n2c.  Meeting Attendance\n\n");
            printf("3.   Print Meeting Schedule\n3a.  FCFS (First Come First Served)\n3b.  SJF (Shortest Job First)\n3c.  YYYY (Attendance Report) \n\n");
            printf("4.   Exit\n\nEnter an option: ");
            fgets(command, 100, stdin);
            strncpy(option, command, 1);
            if (strncmp(option, "1", 1) == 0) {
                while (true) {
                    printf("\nEnter> ");
                    fgets(command, 100, stdin);
                    len = strlen(command);
                    command[len] = 0;       //******************//
                    if (strncmp(command, "0", 1) == 0) {
                        //test_cp_func(staff, project);
                        printf("\n\n\n");
                        break;                       
                    } else {
                        int read_manager[8];
                        for (i = 0; i < 8; i++) {
                            write(toChild[i][1], find_managers, strlen(find_managers) + 1);    //need to add the null character
                        }    
                        for (i = 0; i < 8; i++) {
                            read(toParent[i][0], &read_manager[i], sizeof(read_manager[i]));
                        }  

                        for (i = 0; i < 8; i++) {
                            write(toChild[i][1], pass_data, strlen(pass_data) + 1);    //need to add the null character
                        }

                        for (i = 0; i < 8; i++) {
                            read(toParent[i][0], read_data[i], sizeof(read_data[i]));     //not sure if +1 *****************************//
                        }

                        create_project_team(toChild, command, len, read_manager, read_data);
                    }
                }

            } else if (strncmp(option, "2", 1) == 0) {

                for (i = 0; i < 8; i++) {
                    write(toChild[i][1], pass_data, strlen(pass_data) + 1);    //need to add the null character
                }

                for (i = 0; i < 8; i++) {
                    read(toParent[i][0], read_data[i], sizeof(read_data[i]));     //not sure if +1 *****************************//
                }           
                // for(i = 0; i<8; i++){
                //     for(j=0; j<5; j++){
                //         printf("%d ", read_data[i][j]);
                //     }
                //     printf("\n");
                // }    
                char useful_inf[30];
                while (true) {
                    printf("\nEnter> ");
                    fgets(command, 100, stdin);
                    len = strlen(command);
                    command[len] = 0;    //**************************//
                    if (strncmp(command, "For 2a", 6) == 0) {
                        strncpy(useful_inf, command + 8, 30);
                        if (single_input_meeting_request(toChild, useful_inf) < 0) {

                        } else {
                            printf(">>>>>>Project Meeting request has been accepted\n");
                        }
                    } else if (strncmp(command, "For 2b", 6) == 0) {
                        strncpy(useful_inf, command + 8, 30);
                        int record_num = 0;
                        record_num = batch_input_meeting_request(toChild, useful_inf);
                        printf(">>>>>> %d valid meeting requests have been recorded\n", record_num);
                    } else if (strncmp(command, "For 2c", 6) == 0) {
                        strncpy(useful_inf, command + 8, 30);
                        FCFS(useful_inf, read_data, "MAR");
                        SJF(useful_inf, read_data, "MAR");
                    } else if (strncmp(command, "0", 1) == 0) {
                        break;
                    } 
                }
            } else if (strncmp(option, "3", 1) == 0) {
                char useful_inf[30];
                while (true) {
                    printf("\nEnter> ");
                    fgets(command, 100, stdin);
                    len = strlen(command);
                    command[len] = 0;    //**************************//
                    if (strncmp(command, "For 3a", 6) == 0) {
                        strncpy(useful_inf, command + 13, 30);
                        FCFS(useful_inf, read_data, "");    //removed pipes
                    } else if (strncmp(command, "For 3b", 6) == 0) {
                        strncpy(useful_inf, command + 13, 30);
                        SJF(useful_inf, read_data, "");    //removed pipes
                    } else if (strncmp(command, "For 3c", 6) == 0) {
                        strncpy(useful_inf, command + 12, 30);
                        FCFS(useful_inf, read_data, "SAR");
                        SJF(useful_inf, read_data, "SAR");
                    } else if (strncmp(command, "0", 1) == 0) {
                        break;
                    }
                }
            } else if (strncmp(option, "4", 1) == 0) {
                for (i = 0; i < 13; i++) {
                    char temp[5];
                    strcpy(temp, exit_PMS);
                    write(toChild[i][1], temp, strlen(temp) + 1);    //need to add the null character
                }
                remove("Input_Meeting.txt");
                break;
            } else {
                printf("\nThe comment is illegal. Please re-enter the comment.\n");
            }
        }
        for(i=0; i<CHILD_NUMBER; i++){
            close(toChild[i][1]);
            close(toChild[i][0]);
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

void create_project_team(int fd[13][2], char *command, int len, int read_manager[8], int read_data[8][5]) {
    char staffName[8][10] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};
    char teamName[5][10] = {"Team_A", "Team_B", "Team_C", "Team_D", "Team_E"};
    char useful_inf[6];
    char **res = split(command, " ");
    int i=0, j=0;
    int k, n;

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
    int infosize = i;
    int manager = useful_inf[2] - 'A'; // 
    int team = useful_inf[0] - 'A'; //
    int project = useful_inf[1] - 'A'; //
    int team_no=0;
    
    /*
    Check staff occupy
    */
    
    // 1. check if the prompt consists of more than 4 members.
    if (infosize > 6){
        if (infosize == 3) {
            printf("Project Team must be consists of at least one project member.\n");
        }
        else {
            printf("Project team can not be consists of more than 4 staff members.\n");
        }
        return;
    }
    
    // 2. check if the prompt consists of duplicate information.
    for (i = 0; i < infosize; ++i) {
        for (j = i+1 ; j < infosize; ++j) {
            if ( strncmp(res[i],res[j], strlen(res[i])) == 0 ) {
                if (i == 2) {
                    printf("Manager cannot be a project member of the team.\n");
                }
                else {
                    printf("Prompt consists of duplicate elements. Please try again!\n");
                }
                return;
            }
        }
    }

    // 3. check if the manager is the manager of other project
    //printf("manager %d team %d\n", manager, team);
    //printf("read_manager[manager] %d\n", read_manager[manager]);
    if (read_manager[manager] != -1){
        // if not -1, it is already a manager of other project
        printf("Staff member %s is already a manager of %s\n\n", staffName[manager], teamName[read_manager[manager]]);
        return;
    }
    

    // 4. check staff member participation count
    // if manager is already participated in 3 projects,

    if(proj_participation[manager] >= 3){
        printf("Staff member %s is already a member of 3 other teams\n\n", staffName[manager]);
        return;          
    }      

    if (is_team_created[team] != 0) {
        printf("Team %c has already been created, try a different team\n\n", useful_inf[0]);
        return;
    }
    
    if (is_project_created[project] != 0) {
        printf("Project %c has already been created, try a different project\n\n", useful_inf[1]);
        return;
    }

    // if staff is already participated in 3 projects,
    for (i = 0; i<3; i++){
        // calculate the position of staff in array
        int pos = useful_inf[3+i] - 'A';
        if (pos < 0){continue;}
        if (proj_participation[pos] >= 3){
            printf("Staff member %s is already a member of 3 other teams\n\n", staffName[pos]);
            return;
        }//printf("participation count for %s is %d\n", staffName[pos], proj_participation[pos]);
    }


    // if pass both requirements, can create project
    // increment project participation count for manager and count
    proj_participation[manager]++;
    is_team_created[team] = 1;
    is_project_created[project] = 1;
    for (i = 0; i<3; i++){
        int pos = useful_inf[3+i] - 'A';
        if (pos < 0) {continue;}
        proj_participation[pos]++;
    }
   

    int useful_inf_len = 8;
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
    for (i = 2; i < useful_inf_len; i++) {
        if (i == 2) {
            write(fd[useful_inf[i] - 'A'][1], to_manager_message, strlen(to_manager_message) + 1);    //need to add the null character
        } else {
            write(fd[useful_inf[i] - 'A'][1], to_member_message, strlen(to_member_message) + 1);     //need to add the null character
        }
    }
    write(fd[useful_inf[1] - 'A' + 8][1], to_project_message, strlen(to_project_message) + 1);
    sleep(1);
    printf("\n>>>>>> Project Team %c is created.\n", res[0][5]);
}


int single_input_meeting_request(int fd[13][2], char useful_inf[30]) {
    FILE *input_file;
    input_file = fopen("Input_Meeting.txt", "a");
    char team[2], day[11], start_time[6], hours[2];

    char start_date[11];
    char end_date[11];
    int len, start = 0, duration = 0, end = 0;
    strcpy(start_date, "2022-04-25");  //record the starting date
    strcpy(end_date, "2022-05-14");   //record the end date
    len = strlen(start_date);   
    start_date[len-1] = '\0';    
    len = strlen(end_date);
    end_date[len-1] = '\0';

    strncpy(team, useful_inf + 5, 1);
    strncpy(day, useful_inf + 7, 10);
    strncpy(start_time, useful_inf + 18, 5);
    strncpy(hours, useful_inf + 24, 1);
    // if (!is_valid_day(day)) {
    //     printf("It has an invalid date\n");
    //     return -1;
    // }
    if (!is_valid_time(start_time)) {
        printf("It has an invalid start time\n");
        return -1;
    }
    if(strcmp(day, start_date) < 0 || strcmp(day, end_date) > 0){
        printf("It has an invalid date\n");
        return -1;       
    }

    start = atoi(start_time);
    duration = atoi(hours);
    end = start + duration;
    if(end > 18){
        printf("It has an invalid end time\n");
        return -1;
    }
    if(!is_valid_duration(duration)){
        printf("It has an invalid duration\n");
        return -1;        
    }

    fprintf(input_file, "%s\n", useful_inf);
    fclose(input_file);
    return 1;
}


int batch_input_meeting_request(int fd[13][2], char *command) {
    char useful_inf[30];
    char toInputFile[30];
    strcpy(useful_inf, command);
    strcpy(toInputFile, "For 2b, ");
    int i = 0, line_num = 0, len = 0;
    char buf[MAX_LINE];
    char use_inf[30];
    FILE *fp;
    useful_inf[strlen(useful_inf) - 1] = '\0';          //Maybe not -1//
    if ((fp = fopen(useful_inf, "r")) == NULL) {
        printf("fail to read the file\n");
        return -1;
    }

    while (fgets(buf, MAX_LINE, fp) != NULL) {
        len = strlen(buf);
        buf[len] = '\0';    //Maybe not -1//
        strcpy(use_inf, buf);
        //strcat(toInputFile, use_inf);
        if (single_input_meeting_request(fd, use_inf) > 0) {
            line_num++;
        }else{
            printf("This project meeting request is invalid: %s\n", use_inf);
        }
    }
    fclose(fp);
    return line_num;
}




void meeting_attendance_request(char start_date[11], char end_date[11], int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm) {
    int i, j, k, l;
    int accepted_length, rejected_length; //number of scheduled and rejected requests
    int total_hours, total_meeting;
    float average;
    char algorithm_used[10];
    char teamName[5][10] = {"Team_A", "Team_B", "Team_C", "Team_D", "Team_E"};
    float max_average = 0.0, min_average = 100.0;
    char max_Team, min_Team;

    strcpy(algorithm_used, algorithm);
    algorithm_used[strlen(algorithm_used)] = '\0';

    accepted_length = index1;
    rejected_length = index2; 

    FILE *fp;
    if(strcmp(algorithm_used, "FCFS") == 0){
        fp = fopen("MAR.txt", "w+");
        fprintf(fp, "*** Meeting Attendance Report *** \n\n");
    }
    else if(strcmp(algorithm_used, "SJF") == 0){
        fp = fopen("MAR.txt", "a");
    }

    if (fp == NULL) {
        fprintf(stderr, "Failed to open the file");
        return;
    }

    fprintf(fp, "Algorithm used: %s \n", algorithm);
    fprintf(fp, "====================================================================================== \n");  

    for (i = 0; i < 5; i++) {
        fprintf(fp, "For Project Team_ %c\n", teamName[i][5]);
        total_meeting = 0;
        total_hours = 0;

        for(l=0; l<accepted_length; l++){
            int num = accepted_meetings[l][1][5] - 'A';
            if(num == i){
                j=0;
                total_hours += atoi(accepted_meetings[l][j+3]);
                total_meeting++;
            }
        }
        average = (float)total_hours / (float)total_meeting;

        if(average > max_average){
            max_average = average;
            max_Team = teamName[i][5];
        }
        if(average < min_average){
            min_average = average;
            min_Team = teamName[i][5];
        }
        fprintf(fp, "Team_%c will have an average duration of %.1f hours for each of their meetings\n\n", teamName[i][5], average);
    }  
    fprintf(fp, "====================================================================================== \n");

    fprintf(fp, "Out of all the teams, Project Team_%c will have the highest average duration of %.1f hours for each of their meetings\n", max_Team, max_average);
    fprintf(fp, "And, Project Team_%c will have the lowest average duration of %.1f hours for each of their meetings\n", min_Team, min_average);

    int accepted_hours=0;
    int rejected_hours=0;
    for (i = 0; i < accepted_length; i++){
        accepted_hours += atoi(accepted_meetings[i][3]);
    }
    for (i = 0; i < rejected_length; i++){
        rejected_hours += atoi(rejected_meetings[i][3]);
    }
    float total_utilization = ((float)(accepted_hours + rejected_hours) / 162.0) * 100.0; 
    if(strcmp(algorithm_used, "FCFS") == 0){
        store_util[0] = total_utilization;
        store_util[1] = (float)rejected_length;
    }

    if(strcmp(algorithm_used, "SJF") == 0){
        int fcfs_util = (int)store_util[1];
        fprintf(fp, "====================================================================================== \n");
        fprintf(fp, "Algorithm \"%s\" will have an utilization rate of %.3f%% during the time period from %s to %s\n\n", "FCFS", store_util[0], start_date, end_date);
        fprintf(fp, "Algorithm \"%s\" will have an utilization rate of %.3f%% during the time period from %s to %s\n\n\n", "SJF", total_utilization, start_date, end_date);

        fprintf(fp, "Algorithm \"%s\" rejected a total of %d meetings during the time period from %s to %s\n\n", "FCFS", fcfs_util, start_date, end_date);
        fprintf(fp, "Algorithm \"%s\" rejected a total of %d meetings during the time period from %s to %s\n\n\n", "SJF", rejected_length, start_date, end_date);

        if(store_util[0] < total_utilization){
            fprintf(fp, "Here, Algorithm \"%s\" has a higher utilization rate than Algoritm \"%s\" in scheduling the meetings\n", "SJF", "FCFS");
            fprintf(fp, "As a result, Algorithm \"%s\" is better in this instance to use, for achieving a higher timeslot utilization rate than Algorithm \"%s\"\n\n", "SJF", "FCFS");
        }else if(store_util[0] > total_utilization){
            fprintf(fp, "Here, Algorithm \"%s\" has a higher utilization rate than Algoritm \"%s\" in scheduling the meetings\n", "FCFS", "SJF");  
            fprintf(fp, "As a result, Algorithm \"%s\" is better in this instance to use, for achieving a higher timeslot utilization rate than Algoritm \"%s\"\n\n", "FCFS", "SJF");          
        }else{
            fprintf(fp, "Here, both of the Algorithm \"%s\" and Algoritm \"%s\" have the same utilization rate in scheduling the meetings\n\n", "FCFS", "SJF");
        }


        if(fcfs_util < rejected_length){
            fprintf(fp, "Also, Algorithm \"%s\" rejected a higher number of meetings than Algorithm \"%s\"\n", "SJF", "FCFS");
            fprintf(fp, "As a result, Algorithm \"%s\" is better in this instance to schedule as many meetings as possible than compared to Algorithm \"%s\"\n\n", "FCFS", "SJF");

        }else if(fcfs_util > rejected_length){
            fprintf(fp, "Also, Algorithm \"%s\" rejected a higher number of meetings than Algorithm \"%s\"\n", "FCFS", "SJF"); 
            fprintf(fp, "As a result, Algorithm \"%s\" is better in this instance to schedule as many meetings as possible than compared to Algorithm \"%s\"\n\n", "SJF", "FCFS");
        }else{
            fprintf(fp, "Here, both of the Algorithm \"%s\" and Algoritm \"%s\" rejected the same number of meetings\n\n", "FCFS", "SJF");
        }

        fprintf(fp, "====================================================================================== \n");
    }

    if(strcmp(algorithm_used, "SJF") == 0){
        printf("Printed. Export file name: %s\n", "MAR.txt");
    }
    fclose(fp);
    return;
}



void analyse_attendance(char useful_inf[30], char start_date[11], char end_date[11], int time_period, int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, char *algorithm) {
    int i, j, k, l;
    int accepted_length, rejected_length; //number of scheduled and rejected requests
    int staffHour, team_no, total_meeting;
    float attendance;
    float max_attendance = 0.0, min_attendance = 100.0; 
    float attendance_rate[8];
    char algorithm_used[10];
    char staffName[8][10] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};

    strcpy(algorithm_used, algorithm);
    algorithm_used[strlen(algorithm_used)] = '\0';

    accepted_length = index1;
    rejected_length = index2; 

    FILE *fp;
    if(strcmp(algorithm_used, "FCFS") == 0){
        fp = fopen("SAR.txt", "w+");
        fprintf(fp, "*** Staff Attendance Report *** \n\n");
    }
    else if(strcmp(algorithm_used, "SJF") == 0){
        fp = fopen("SAR.txt", "a");
    }

    if (fp == NULL) {
        fprintf(stderr, "Failed to open the file");
        return;
    }

    fprintf(fp, "Algorithm used: %s \n", algorithm);
    fprintf(fp, "====================================================================================== \n");  

    for (i = 0; i < 8; i++) {
        fprintf(fp, "Staff: %s \n\n", staffName[i]);
        team_no = 0;
        staffHour = 0;
        total_meeting = 0;

        for(j=0; j<5; j++){
            if(read_data[i][j] != -1){
                team_no++;
            }
        }

        for(l=0; l<accepted_length; l++){
            int num = accepted_meetings[l][1][5] - 'A';
            for(k=0; k<5; k++){
                if(read_data[i][k] == -1){
                    continue;
                }
                else if(read_data[i][k] == num){
                    j=0;
                    staffHour += atoi(accepted_meetings[l][j+3]); 
                    total_meeting++;
                    break;
                }
            }
        }
        attendance = ((float)staffHour / (float)time_period) * 100.0;
        attendance_rate[i] = attendance;

        fprintf(fp, "%s will attend a total of %d meetings as being part of %d different teams\n", staffName[i], total_meeting, team_no);
        fprintf(fp, "%s will have an attendace rate of %.1f%% during the time period from %s to %s\n\n", staffName[i], attendance, start_date, end_date);
    }  
    fprintf(fp, "====================================================================================== \n");

    float temp;
    char temp_staff[10];
    char temp_staffName[8][10] = {"Alan", "Billy", "Cathy", "David", "Eva", "Fanny", "Gary", "Helen"};
    for(i=0; i<8; i++){
        for(j=i+1; j<8; j++){
            if(attendance_rate[j] < attendance_rate[i]){
                temp = attendance_rate[i];
                attendance_rate[i] = attendance_rate[j];
                attendance_rate[j] = temp;

                strcpy(temp_staff, temp_staffName[i]);
                strcpy(temp_staffName[i], temp_staffName[j]);
                strcpy(temp_staffName[j], temp_staff);
            }
        }
    }

    max_attendance = attendance_rate[7];
    min_attendance = attendance_rate[0];

    fprintf(fp, "The attendance rates of all the staff from lowest to highest are as follows:\n\n");
    for(i=0; i<8; i++){
        fprintf(fp, "Staff: %s, Attendance rate: %.1f%%\n", temp_staffName[i], attendance_rate[i]);
    }
    fprintf(fp, "====================================================================================== \n");
    fprintf(fp, "\nHere, Staff %s has the highest attendance rate of %.1f%% out of everybody else\n", temp_staffName[7], max_attendance);
    fprintf(fp, ">>>>>> There is a very high chance that %s will always arrive at the meetings on time, and may never be late or not show up\n", temp_staffName[7]);
    fprintf(fp, ">>>>>> And this might also be an indication that %s might be the best performing member in all of the project teams that he is a part of\n\n", temp_staffName[7]);
   
    fprintf(fp, "====================================================================================== \n");
    fprintf(fp, "\nHere, Staff %s has the lowest attendance rate of %.1f%% out of everybody else\n", temp_staffName[0], min_attendance);
    fprintf(fp, ">>>>>> There is a very high chance that %s will always arrive late at the meetings and very rarely show up on time\n", temp_staffName[0]);
    fprintf(fp, ">>>>>> And this might also be an indication that %s might be the worst performing member in all of the project teams that he is a part of\n\n", temp_staffName[0]);   
    fprintf(fp, "====================================================================================== \n");

    if(strcmp(algorithm_used, "SJF") == 0){
        printf("Printed. Export file name: %s\n", "SAR.txt");
    }
    fclose(fp);
    return;
}



void FCFS(char useful_inf[30], int read_data[8][5], char *command) {
    int i, j, k, l, day1, day2, time_period=0;
    // 1. read input file
    FILE *fp;
    fp = fopen("Input_Meeting.txt", "r");

    // 2. read project info
    // Team_A 2022-04-25 09:00 2
    // 3. store data
    char meeting_data[162][5][1024];
    char buffer[1024]; char *token=NULL; int idx = 0;


    char check_origin[10];
    strcpy(check_origin, command);
    check_origin[strlen(check_origin)] = '\0';

    while(fgets(buffer, 1024, fp) != NULL){
        if(strcmp(buffer, "\n") != 0 && strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\0") != 0){
            i = 0;
            token = strtok(buffer, " ");

            int start = 0; int duration = 0; int end = 0;
            while (token != NULL){
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
                token = strtok (NULL, " ");
            }
            // record the end time
            char endTime[10];
            end = start + duration;
            sprintf(endTime, "%d", end);
            strcpy(meeting_data[idx][4], endTime);        
            idx++;
        }
        else 
        {
            continue;
        }
    }
    //free(token);
    fclose(fp);

    // strip
    for (i = 0; i<idx; i++){   /*************** MAYBE NOT -1 *************/
        meeting_data[i][3][strlen(meeting_data[i][3])] = '\0';    /*************** MAYBE NOT -1 *************/
    }

    char set_meetings[162][5][1024];    //used for keeping all the accepted meetings
    char rejected_meetings[162][5][1024];   //used for keeping all the rejected meetings

    for(i=0; i<162; i++){
        for(j=0; j<5; j++){
            strcpy(set_meetings[i][j], "0");    //using "0" as placeholders
            strcpy(rejected_meetings[i][j], "0");
        }
    }

    char start_date[11];
    char end_date[11];
    char num_days[5];
    int len, index1=0, index2=0, track;    //index1 gives the no of accepted meeting requests and index2 gives the no of rejected meeting requests

    strcpy(start_date, "2022-04-25");  //record the starting date
    strcpy(end_date, "2022-05-14");   //record the end date   
    start_date[strlen(start_date)] = '\0';    
    end_date[strlen(end_date)] = '\0';


    if(strcmp(check_origin, "MAR") != 0){
        strncpy(start_date, useful_inf, 10);   //record the starting date
        start_date[strlen(start_date)] = '\0';
        strncpy(end_date, useful_inf + 11, 10);    //record the end date
        end_date[strlen(end_date)] = '\0';

        //Finding the no of days for the scheduling
        strncpy(num_days, start_date+8, 2);
        num_days[strlen(num_days)] = '\0';
        day1 = atoi(num_days);

        strncpy(num_days, end_date+8, 2);
        num_days[strlen(num_days)] = '\0';
        day2 = atoi(num_days);

        if(day2 >= day1){
            time_period = ((day2 - day1) + 1) * 9;
        }else{
            time_period = ((30 - day1 + 1) + day2) * 9;
        }
    }


    for(i=0; i<idx; i++){
        track = 1;
        for(j=0; j<5; j++){
            if(j == 0){
                if(strcmp(meeting_data[i][j], start_date) < 0 || strcmp(meeting_data[i][j], end_date) > 0){   //if date of meeting is outside the allowed meeting schedule
                    track = 0;
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }
            }
            else if(j == 2){
                if(atoi(meeting_data[i][j]) < 9){    //if starting time of meeting less than 09:00
                    track = 0;
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }
            }
            else if(j == 4){     //if ending time of meeting more than 18:00
                if(atoi(meeting_data[i][j]) > 18){
                    track = 0;
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                    }
                    index2++;
                    break;
                }  
                int temp = index1;
                int check = 1;
                for(k=0; k<temp; k++){  
                    //to check for new meetings with the already scheduled meetings
                    if(strcmp(set_meetings[k][0], meeting_data[i][0]) == 0){     //if meeting on the same day
                        track = 0;
                        //if the meeting does not conflict with the time period of an already scheduled meeting
                        if(strcmp(meeting_data[i][2], set_meetings[k][2]) <= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][2]) <= 0){
                            continue;
                        }
                        if(strcmp(meeting_data[i][2], set_meetings[k][4]) >= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][4]) >= 0){
                            continue;
                        }
                        else{
                            check = 0;
                            for(l=0; l<5; l++){
                                strcpy(rejected_meetings[index2][l], meeting_data[i][l]);
                            }
                            index2++; 
                            break;                          
                        }
                    }
                }      
                if(check == 1 || track == 1){
                    for(l=0; l<5; l++){
                        strcpy(set_meetings[index1][l], meeting_data[i][l]);
                    }
                    index1++;                    
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
                for (k = 0; k<5; k++){
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
                for (k=0; k<5; k++){
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
                    for (k = 0; k<5; k++){
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
                    for (k = 0; k<5; k++){
                        strcpy(temp, rejected_meetings[i][k]);
                        strcpy(rejected_meetings[i][k], rejected_meetings[j][k]);
                        strcpy(rejected_meetings[j][k], temp);
                    }
                }
            }
        }
    }

    if(strcmp(check_origin, "SAR") == 0){    //For part 3c
        analyse_attendance(useful_inf, start_date, end_date, time_period, read_data, set_meetings, rejected_meetings, index1, index2, "FCFS");
        return;
    }
    if(strcmp(check_origin, "MAR") == 0){    //For part 2c
        meeting_attendance_request(start_date, end_date, read_data, set_meetings, rejected_meetings, index1, index2, "FCFS");
        return;
    }
    print_schedule(read_data, set_meetings, rejected_meetings, index1, index2, time_period, "FCFS");
    return;
}






void SJF(char useful_inf[30], int read_data[8][5], char *command) {
    int i, j, k, l;
    int day1, day2, time_period=0;

    int rejectedMeetingCount = 0;
    int approvedMeetingCount = 0;

    /*
        Meeting arrays
        - set_meetings: approved meeting list
        - rejected_meetings: rejected meeting list
     */

    char set_meetings[162][5][1024];
    char rejected_meetings[162][5][1024];

    // initialize both arrays
    for(i=0; i<162; i++){
        for(j=0; j<5; j++){
            strcpy(set_meetings[i][j], "0");    //using "0" as placeholders
            strcpy(rejected_meetings[i][j], "0");
        }
    }

    char check_origin[10];
    strcpy(check_origin, command);
    check_origin[strlen(check_origin)] = '\0';

    // 1. read input file
    FILE *fp;
    fp = fopen("Input_Meeting.txt", "r");

    // 2. read project info
    // Team_A 2022-04-25 09:00 2

    // 3. store data
    char meeting_data[162][5][1024];
    char buffer[1024]; char *token; 
    int idx = 0; // number of meeting
    while(fgets(buffer, 1024, fp) != NULL){
        if(strcmp(buffer, "\n") != 0 && strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\0") != 0){
            i = 0;
            token = strtok(buffer, " ");
            int start = 0; int duration = 0; int end = 0;

            while (token != NULL){
                // store date first
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

        }else{
            continue;
        }
    } // end of reading function
    fclose(fp);

    // strip
    for (i = 0; i<idx-1; i++){
        meeting_data[i][3][strlen(meeting_data[i][3])-1] = '\0';
    }

    char start_date[11];
    char end_date[11];
    char num_days[5];

    strcpy(start_date, "2022-04-25");  //record the starting date
    strcpy(end_date, "2022-05-14");   //record the end date   
    start_date[strlen(start_date)] = '\0';    
    end_date[strlen(end_date)] = '\0';


    if(strcmp(check_origin, "MAR") != 0){
        strncpy(start_date, useful_inf, 10);   //record the starting date
        start_date[strlen(start_date)] = '\0';
        strncpy(end_date, useful_inf + 11, 10);    //record the end date
        end_date[strlen(end_date)] = '\0';

        //Finding the no of days for the scheduling
        strncpy(num_days, start_date+8, 2);
        num_days[strlen(num_days)] = '\0';
        day1 = atoi(num_days);

        strncpy(num_days, end_date+8, 2);
        num_days[strlen(num_days)] = '\0';
        day2 = atoi(num_days);

        if(day2 >= day1){
            time_period = ((day2 - day1) + 1) * 9;
        }else{
            time_period = ((30 - day1 + 1) + day2) * 9;
        }
    }

    /**
     * Compare Meeting lists
     * 
     * 1. Date
     * 2. Meeting Start Time
     * 3. Meeting End Time
     * 
     */
    for(i=0; i<idx; i++){
        for(j=0; j<5; j++){
            if(j == 0){ // comparing date (2022-04-25)
                //if date of meeting is outside the allowed meeting schedule
                if(strcmp(meeting_data[i][j], start_date) < 0 && strcmp(meeting_data[i][j], end_date) > 0){ 
                    // store in reject array  
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    rejectedMeetingCount++;
                    break;
                }
            }
            else if(j == 2){ // comparing time (09:00)
                if(atoi(meeting_data[i][j]) < 9){    //if starting time of meeting less than 09:00
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    rejectedMeetingCount++;
                    break;
                }
            }
            else if(j == 4){ //if ending time of meeting more than 18:00 
                if(atoi(meeting_data[i][j]) > 18){
                    for(l=0; l<5; l++){
                        strcpy(rejected_meetings[rejectedMeetingCount][l], meeting_data[i][l]);
                    }
                    rejectedMeetingCount++;
                    break;
                }  
                for(k=0; k<162; k++){ 
                    //to check for new meetings with the already scheduled meetings
                    if(strcmp(set_meetings[k][0], meeting_data[i][0]) == 0){     //if meeting on the same day
                        //if the meeting does not conflict with the time period of an already scheduled meeting
                        if((strcmp(meeting_data[i][2], set_meetings[k][2]) <= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][2]) <= 0) || (strcmp(meeting_data[i][2], set_meetings[k][4]) >= 0  &&  strcmp(meeting_data[i][4], set_meetings[k][4]) >= 0)){
                            for(l=0; l<5; l++){
                                strcpy(set_meetings[approvedMeetingCount][l], meeting_data[i][l]);
                            }
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
     * sort based on duration of meeting
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

    // run SJF
    int endTime = 0; int begin = 0;
    begin = atoi(meeting_data[0][2]);
    for (i = 0; i<idx; i++){
        // until it is not last meeting
        if (i != idx && i != 0){
            // check if the date are same
            // if not, reset the end time
            if (strcmp(meeting_data[i-1][0], meeting_data[i][0]) != 0){
                endTime = 0;
            }
        }

        // extract start time
        int startTime = atoi(meeting_data[i][2]);

        // if end time is bigger than start time, reject
        // e.g) previous meeting ends at 11:00AM and next meeting starts at 9:00AM
        if (endTime > startTime){
            rejectedMeetingCount++;

            // copy meeting info as rejected
            for (j = 0; j<5; j++){
                strcpy(rejected_meetings[rejectedMeetingCount-1][j], meeting_data[i][j]);

            }
        }

        // if not,
        else{ // approved meeting
            // copy meeting info as approved
            for (k = 0; k<5; k++){
                strcpy(set_meetings[approvedMeetingCount][k], meeting_data[i][k]);
            }

            approvedMeetingCount++;
            endTime = atoi(meeting_data[i][4]);
        }

    }

    // approvedMeeting : number of meeting - rejected meeting
    approvedMeetingCount = idx - rejectedMeetingCount;

    if(strcmp(check_origin, "SAR") == 0){    //For part 3c
        analyse_attendance(useful_inf, start_date, end_date, time_period, read_data, set_meetings, rejected_meetings, approvedMeetingCount, rejectedMeetingCount, "SJF");
        return;
    }
    if(strcmp(check_origin, "MAR") == 0){    //For part 2c
        meeting_attendance_request(start_date, end_date, read_data, set_meetings, rejected_meetings, approvedMeetingCount, rejectedMeetingCount, "SJF");
        return;
    }
    print_schedule(read_data, set_meetings, rejected_meetings,  approvedMeetingCount, rejectedMeetingCount, time_period, "SJF");
    return;
}



int extract_int(char *s, int start, int len) {
    char ss[5];
    strncpy(ss, s + start, len);
    ss[len] = 0;
    return atoi(ss);
}

bool is_valid_time(char *s1) {
    int time;
    time = extract_int(s1, 0, 2);
    if (time < 9 || time > 18) {
        return false;
    }
    return true;
}

bool is_valid_duration(int x){
    if(x < 1 || x > 9){
        return false;
    }
    return true;
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


void print_schedule(int read_data[8][5], char accepted_meetings[162][5][1024], char rejected_meetings[162][5][1024], int index1, int index2, int time_peroid, char *algorithm) {    
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

    int result = sprintf(filename, "Schedule_%s_%02d.txt",algorithm, file_num);

    int teams[5];

    file_num++;
    
    fp = fopen(filename, "w+");
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


    float total_utilization = ((float)(accepted_hours + rejected_hours) / (float)time_peroid) * 100.0; 
    float accepted_utilization = ((float)accepted_hours / (float)time_peroid) * 100.0;
    float rejected_utilization = ((float)rejected_hours / (float)time_peroid) * 100.0;
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
        accepted_utilization = ((float)accepted_hours / (float)time_peroid) * 100.0;
        fprintf(fp, "%4s %s %20s - %.1f%%\n", blanks, teamName[i], blanks, accepted_utilization);
    }

    
    for (i = 0; i < 8; i++){
        float staff_Utilization = ((float)staffHours[i] / (float)time_peroid) * 100.0;
        fprintf(fp, "%4s Staff_%c %19s - %.1f%%\n", blanks, staffName[i][0], blanks, staff_Utilization);        
    }
    
    fclose(fp);
    printf("Printed. Export file name: %s\n", filename);

    return;
}
