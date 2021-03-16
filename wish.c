#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

struct Commands {
  char **commands;
  int numCommands;
};

char** createArray(int m, int n) {
  char **arr=(char **)malloc(sizeof(char *)*m);
  int i;
  for(i=0;i<m;i++)
  {
    arr[i]=(char *)malloc(sizeof(char)*n);
  }
  return arr;
}

void destroyArray(char** arr)
{
  free(*arr);
  free(arr);
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}


int stringsAreEqual(const char first[], const char second[]) {
  return (!strcmp(first, second));
}

// Checks whether a string contains a symbol
int contains(const char string[], const char symbol) {
  for (int i = 0; i < strlen(string); i++) {
    if (string[i] == symbol) return 1;
  }
  return 0;
}

// Counts occurences of symbols in a string
int count(const char string[], const char symbols[]) {
  int output = 0;
  for (int i = 0; i < strlen(string); i++) {
    if (contains(symbols, string[i])) {
      output++;
    }
  }
  return output;
}

// Splits a string into a list of strings
struct Commands split(const char string[], const char delimit[]) {
  int numWords = count(string, delimit) + 1; // 1 + number of delimiters
  char **commands = createArray(numWords, 10);
  int j = 0;
  int count = 0;
  for (int i = 0; i < strlen(string); i++) {
    // if delimiter, append \0 to finish word, and then start a new word
    if(contains(delimit, string[i])) {
      commands[count][j]='\0';
      count++;
      j=0;
    }
    // else just add the char to the current word
    else
    {
      commands[count][j]=string[i];
      j++;
    }
  }
  struct Commands output;
  output.commands = commands;
  output.numCommands = numWords;
  return output;
}

void printWords(struct Commands words) {
  printf("%s", words.commands[0]);
  for (int i = 1; i < words.numCommands; i++) { 
    printf(", %s", words.commands[i]);
  }
  printf("\n");
}

void executeCommand(struct Commands input) {
  int pid = fork();
  if (pid == 0) {
    int error = execvp(input.commands[0], input.commands);
    printf("Could not execute command. Error: %d\n", error);
  }
  int *foo;
  wait(foo);
}

void main(int argc, char **argv) {
  const char delimiter[] = " \t";
  char input[256];
  while (1) {
    printf("> ");
    fgets(input, sizeof(input), stdin);
    trimwhitespace(input);
    struct Commands commands = split(input, delimiter);
    executeCommand(commands);
    destroyArray(commands.commands);
  }
}
