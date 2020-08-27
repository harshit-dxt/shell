#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>

#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define flush() while(getchar()!='\n');

void printError(){
  printf("Error: %s\n", strerror(errno));
}
void executeBuiltin(char* cmd, char** args){
  if(!strcmp(cmd,"exit")){
    printf("Closing the shell..\n");
    printf("Bye! %s\n", getenv("USER"));
    exit(0);
  }
  else if(!strcmp(cmd,"pwd")){
    char cwd[MAX_PATH_SIZE];
    if(getcwd(cwd, sizeof(cwd))!=NULL){
      printf("%s\n",cwd);
    }
    else{
      printError();
    }
  }
  else if(!strcmp(cmd, "ls")){
    struct dirent *de;
    DIR *dr = opendir(".");

    if (dr == NULL)
    {
        printf("Could not open current directory" );
    }
    while ((de = readdir(dr)) != NULL)
            printf("%s\n", de->d_name);
    closedir(dr);
  }
  else if(!strcmp(cmd, "cd")){
    if(chdir(args[1])){
      printError();
    }
  }
  else if(!strcmp(cmd,"history")){
    printf("exexuted\n");
    FILE* fp = fopen("history.txt", "r+");
    char c = fgetc(fp);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fp);
    }
    fclose(fp);
  }
  else if(!strcmp(cmd,"mkdir")){
    if(mkdir(args[1], 700)){
      printError();
    }
    else{
        printf("Directory created :) \n");
    }
  }
  else if(!strcmp(cmd, "echo")){
    int count = 1;
    while(args[count]!=NULL){
      printf("%s ",args[count]);
      count++;
    }
    printf("\n");
  }
  else if(!strcmp(cmd, "rmdir")){
    if(rmdir(args[1])){
      printError();
    }
    else{
      printf("Removed directory %s\n", args[1]);
    }
  }
  else if(!strcmp(cmd, "kill")){
    int stat;
    kill ((pid_t)atoi(args[1]),SIGUSR1);
    wait(&stat);
  }
}

int isBuiltinCmd(char* cmd){
  if(!(strcmp(cmd,"exit")&&strcmp(cmd, "ls")&&strcmp(cmd, "pwd")&&strcmp(cmd, "cd")&&strcmp(cmd, "echo")&&strcmp(cmd, "mkdir")&&strcmp(cmd, "history")&&strcmp(cmd, "rmdir")&&strcmp(cmd, "kill"))){
    return 1;
  }
  return 0;
}
int printenv(char* env){
  char* line = getenv(env);
  if(line){
    printf("%s: %s\n", env, line);
    return 1;
  }
  else{
    return 0;
  }
}
void printPrompt(){
  char cwd[MAX_PATH_SIZE];
  if(getcwd(cwd, sizeof(cwd))!=NULL){
    printf("%s@%s:~$ ", getenv("USER")?getenv("USER"):"", cwd);
  }
  else{
    printf("Error: %s\n",strerror(errno));
  }
}

int tokenise(char* line, char* delimiter, char** tokens, int* n_cmd, int* background){
  int n_tokens = 0;
  *n_cmd = 1;
  char* token = strtok(line, delimiter);
  while(token!= NULL){
    if(!strcmp(token, "|")) (*n_cmd)++;
    if(!strcmp(token,"&")){ *background = 1;}
      tokens[n_tokens] = token;
      token = strtok(NULL, delimiter);
      n_tokens++;
  }
  return n_tokens;
}

void historycmd(char* line){
  FILE* fp = fopen("history.txt","a");
  fprintf(fp, "%s\n", line);
  fclose(fp);
}

int readLine(char* line){
  int success = 0;
  scanf("%[^\n]", line);
  line[strlen(line)] = '\0';
  if(success = strlen(line)) historycmd(line);
  flush();
  return success;
}

int parseCmd(char** tokens, int n_tokens, int n_cmd, int background){
  pid_t child = 0;
  int cmd_exec = 0;
  if(isBuiltinCmd(tokens[0])){
    executeBuiltin(tokens[0], tokens);
    cmd_exec = 1;
  }
  else if(printenv(tokens[0])){
    cmd_exec = 1;
  }
  else if(n_cmd<2){
    child = fork();
    if(child < 0){
      printError();
    }
    else if(!child) {
      if (execvp(tokens[0],tokens) < 0) {
        return 0;
      }
      exit(0);
    }
    else{
      if(!background)
      waitpid(child, NULL, 0);
    }
  }
}


int main(int argc, char** argv){
  char line[MAX_INPUT_SIZE];


  FILE* fp;
  if(argc == 2){
    fp = fopen(argv[1],"r");
    if(fp < 0){
      printf("File doesn't exist.\n");
      return -1;
    }
  }
  while(1){
    memset(line, 0, sizeof(line));
    if(argc == 2){
      if(fgets(line, sizeof(line), fp)==NULL){
        break;
      }
      line[strlen(line)-1] = '\0';
    }
    else{

      int n_tokens = 0, n_cmd=0, background = 0;
      printPrompt();
      if(readLine(line)){
        char* tokens[MAX_TOKEN_SIZE];
        n_tokens = tokenise(line, " ", tokens,&n_cmd, &background)+1;
        if(strcmp(tokens[n_tokens-2],"&") == 0){
          tokens[n_tokens-2] = NULL;
        }
        parseCmd(tokens, n_tokens, n_cmd, background);
      }
    }
  }
  fclose(fp);
  return 0;
}
