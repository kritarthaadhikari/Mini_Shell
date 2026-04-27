#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>

int shellRunning = 1;
int ExecuteCommand(char **);

void AddHistory(char* command){

    FILE *old = fopen("history.txt", "r");
    FILE *backup = fopen("history_temp.txt", "w");

    char line[256];

    fprintf(backup,"%s",command);

    while (fgets(line, sizeof(line), old)) {
        fprintf(backup, "%s", line);
    }

    fclose(old);
    fclose(backup);

    remove("history.txt");
    rename("history_temp.txt","history.txt");

}


void ParseInput(char* command, char **output){

    int i = 0;
    int j = 0;

    char buffer[32][64];

    for (int k = 0; command[k] != '\0'; k++) {

        if (command[k] == ' ' || command[k] == '\n') {
            if (j > 0) {
                buffer[i][j] = '\0';     // Null terminate word
                output[i] = buffer[i];   // Store
                i++;
                j = 0;
            }
        }
        else {
            buffer[i][j] = command[k];
            j++;
        }
    }

    if (j > 0) {
        buffer[i][j] = '\0';
        output[i] = buffer[i];
        i++;
    }

    output[i] = NULL;
}

int HelpCommand(char **command){
    if (command[2]!=NULL && command[1]!=NULL){
        printf("[ERROR] 'help' takes 0 or 1 arguments but more were given.\n");
        return 1;
    }else{
        FILE *fp = fopen("help.txt","r");
        char line[256];
        char helpCommand[32];
        int printed = 0;


        while (fgets(line, sizeof(line), fp) != NULL) {
            int i=0;
            while(line[i] != ' '){
                helpCommand[i] = line[i];
                i++;
            }
            helpCommand[i] = '\0';
            if (command[1]==NULL || strcmp(helpCommand,command[1])==0  ){
                printf("%s",line);
                printed = 1;
            }
        }
        if (printed==0 && command[1]!=NULL){
            printf("[ERROR]: '%s' command not found.",command[1]);
            return 1;
        }
        fclose(fp);
        return 0;
    }
}

int HistoryCommand(char **command){
    if (command[1] != NULL){
        printf("[ERROR]: 'history' takes no arguments but they were given.\n");
        return 1;
    }else{
        FILE *fp = fopen("history.txt", "r");
        char line[100];
        int i = 0;
        while(fgets(line,sizeof(line),fp)!=NULL){
            printf("[%d] - %s",i,line);
            i++;
        }

        fclose(fp);
        return 2;
    }
}

int CatCommand(char **command){
    //command [1] will have the filepath
     if(command[1] == NULL){
        printf("[ERROR]: 'cat' requires a file path.\n");
        return 1;
    }

    if(command[2] != NULL){
        printf("[ERROR]: 'cat' takes only one argument.\n");
        return 1;
    }

    FILE *fp = fopen(command[1], "r");

    if(fp == NULL){
        printf("[ERROR]: File '%s' not found.\n", command[1]);
        return 1;
    }

    char ch;

    while((ch = fgetc(fp)) != EOF){
        putchar(ch);
    }

    fclose(fp);
    return 0;
}

int RepeatCommand(char **command){
    if(command[1]==NULL || command[2]!= NULL){
        printf("[ERROR]: 'repeat' command takes a single argument.\n");
        return 1;
    }else{
        int index = atoi(command[1]);
        FILE *fp = fopen("history.txt","r");
        int counter = 0;
        char line[256];
        while(fgets(line,sizeof(line),fp)!=NULL){
            if (counter==index) break;
            counter++;
        }
        char *parsedInput[32];
        ParseInput(line,parsedInput);
        int temp = ExecuteCommand(parsedInput);
        return 2;
    }
}
int ClearCommand(char **command){
    if(command[1]!=NULL){
        printf("[ERROR]: 'clear' command takes no argument but they were given.\n");
        return 1;
    }
    system("cls");
    return 2;
}

int PwdCommand() {
    char path[256];
    // getcwd fills our 'path' array with the current location
    if (getcwd(path, sizeof(path)) != NULL) { //getcwd stands for "get current working directory"
        printf("%s\n", path);
        return 0;
    } else {
        perror("getcwd() error"); //perror- abbreviated(print error)
        return 1;
    }
}

int ExecuteCommand(char **command){

    // Implementation for the exit command
    if (strcmp(command[0],"exit")==0){
        int i = 1;
        for(; command[i] != NULL; i++){
        }

        if (i==1){
            shellRunning = 0;
            return 0;
        }else{
            printf("'exit' takes no arguments but %d were given.",i-1);
            return 1;
        }
    }

    // Implementation for the help command
    else if(strcmp(command[0],"help")==0){
        return HelpCommand(command);
    }

    // Implementation of the history command
    else if(strcmp(command[0],"history")==0){
        return HistoryCommand(command);
    }

    else if(strcmp(command[0],"cat")==0){
        return CatCommand(command);
    }

    else if(strcmp(command[0],"repeat")==0){
        return RepeatCommand(command);
    }
    else if (strcmp(command[0],"clear")==0){
        return ClearCommand(command);
    }
    else if(strcmp(command[0],"pwd")==0){
        return PwdCommand();
    }
    else{
        printf ("[ERROR]: '%s' is not a recognized command.\n\n",command[0]);
    }

    return 1;
}

int main(){

    // Character array to store the command
    char command[256];
    char *parsedInput[32];
    int executedValue;

    // Shell entry message
    printf("\nWelcome to Mini Shell!");
    printf("\nType 'help' to see the commands.\n");

    // Shell prompt
    while (shellRunning){
        printf("\n\nmyShell>");
        if(fgets(command, sizeof(command), stdin) == NULL){
            break;
        }
        // Could implement some way to go back to the last function here

        ParseInput(command,parsedInput);

        for(int i = 0; parsedInput[i] != NULL; i++){
            printf("  [%s]\n", parsedInput[i]);
        }

        executedValue= ExecuteCommand(parsedInput);
        if(executedValue == 0){
            AddHistory(command);
        }
        else if(executedValue==1){
             printf("[ERROR]:  failed to execute command: '%s'\n",parsedInput[0]);// Command failure
        }
    }

    printf("Exiting shell...\n\n");
    return 0;
}
