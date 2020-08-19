#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/stat.h>

#define MAX_INPUT_SIZE 1024
#define MAX_PATH_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define flush() while(getchar()!='\n');
struct lineinfo{

}
struct cmdinfo{
  int background;
  int n_tokens;
  int isInternal;
};

struct cmdinfo* init_cmd_info(struct cmdinfo* cmd){
  cmd = (struct cmdinfo*)malloc(sizeof(struct cmdinfo));
  cmd->background = 0;
  cmd->n_tokens = 0;
  cmd->isInternal = 0;
  return cmd;
}

void printenv(char* env){
  char* line = getenv(env);
  if(line){
    printf("%s: %s\n", env, line);
  }
  else{
    return;
  }
}
void printPrompt(){
  char cwd[MAX_PATH_SIZE];
  if(getcwd(cwd, sizeof(cwd))!=NULL){
    printf("%s:~$ ", cwd);
  }
  else{
    printf("Error: %s\n",strerror(errno));
  }
}

int tokenise(char* line, char* delimiter, char** tokens){
  int n_tokens = 0;
  int pipes = 0;
  char* token = strtok(line, delimiter);
  while(token!= NULL){
    if(!strcmp(token, "|")) pipes++;
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

void readLine(char* line){
  scanf("%[^\n]", line);
  line[strlen(line)] = '\0';
  historycmd(line);
  flush();
}

void parseCmd(char** tokens, int n_tokens){
  for(int i = 0;i<n_tokens;i++){
    if(!strcmp(tokens[i],"|"))
  }
}

int main(int argc, char** argv){
  char line[MAX_INPUT_SIZE];
  int n_tokens = 0;
  char* tokens[MAX_TOKEN_SIZE];
  struct cmdinfo** cmd = NULL;
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
      printPrompt();
      readLine(line);
      n_tokens = tokenise(line, " ", tokens)+1;
      parseCmd(tokens, n_tokens);
    }
  }
  fclose(fp);
  return 0;
}
