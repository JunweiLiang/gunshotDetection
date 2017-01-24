#include "str_util.h"

string int2String(int num){
  char buf[STRMAX];

  sprintf(buf, "%d", num);

  return buf;
}

int numField(char* input, char delimiter[]){
  char buffer[STRMAX];
  char* tmp;
  int count=0;

  strcpy(buffer, input);
  tmp = strtok(buffer, delimiter);
  while(tmp!=NULL){
    count++;
    tmp = strtok(NULL, delimiter);
  }

  return count;
}

int extractField(const char* input, int field_id, char delimiter[], char field[]){
  char buffer[STRMAX];
  char* tmp;
  int count=0;

  strcpy(buffer, input);

  tmp = strtok(buffer, delimiter);
  while(count<field_id){
    tmp = strtok(NULL, delimiter);
    if(tmp==NULL){
      sprintf(field, "");
      return 1;
    }
    count++;
  }
  if(tmp==NULL){
    sprintf(field, "");
    return 1;
  }

  strcpy(field, tmp);

  return 0;
}

int match(char* input, char* reg_expr){
  regex_t preg;
  regmatch_t pmatch;
  int code;

  code = regcomp(&preg, reg_expr, REG_NEWLINE);
  if(code!=0){
    printf("Can't compile the regular expression: %s\n", reg_expr);
    return 0;
  }

  code = regexec(&preg, input, 1, &pmatch, 0);
  regfree(&preg);
  
  if(code==0)
    return 1;
  return 0;
}

int chomp(char* input){
  int length = strlen(input);
  if(input[length-1]=='\n'){
    input[length-1] = 0;
    return 1;
  }
  return 0;
}
