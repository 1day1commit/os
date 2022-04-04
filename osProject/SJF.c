
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
        char endTime[4];
        end = start + duration;
        
        
        //meeting_data[idx][4][strcspn(meeting_data[idx][4], "\n")] = '\0';
        // meeting_data[idx][4][strlen(meeting_data[idx][4])-1] = '\0';
        // endTime[strlen(endTime)-1] = '\0';
        strcpy(meeting_data[idx][4], endTime);
        idx++;
    }
    fclose(fp);
    // print
    for(i = 0; i<idx; i++){
        for (j = 0; j<5; j++){
            printf("%s ",  meeting_data[i][j]);
        }printf("\n");
    }


    
    printf("done\n");

    return;
}
