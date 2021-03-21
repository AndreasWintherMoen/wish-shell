#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct Tokens {
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
struct Tokens split(const char string[], const char delimit[]) {
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
  struct Tokens output;
  output.commands = commands;
  output.numCommands = numWords;
  return output;
}

void printWords(struct Tokens words) {
  printf("%s", words.commands[0]);
  for (int i = 1; i < words.numCommands; i++) { 
    printf(", %s", words.commands[i]);
  }
  printf("\n");
}

int isOutputRedirection(char *str) {
  return stringsAreEqual(str, ">");
}

int isInputRedirection(char *str) {
  return stringsAreEqual(str, "<");
}

int findOutputRedirection(struct Tokens input) {
  for (int i = 0; i < input.numCommands; i++) {
    if (isOutputRedirection(input.commands[i])) {
      return i;
    }
  }
  return -1;
}

int findInputRedirection(struct Tokens input) {
  for (int i = 0; i < input.numCommands; i++) {
    if (isInputRedirection(input.commands[i])) {
      return i;
    }
  }
  return -1;
}

void redirectOutputToFile(char *filename) {
  int fd = creat(filename, 0644);
  if (fd < 0) {
    printf("Could not open file %s. Error %d", filename, fd);
  }
  dup2(fd, STDOUT_FILENO);
  close(fd);
}

void redirectInputToFile(char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    printf("Could not open file %s. Error %d", filename, fd);
  }
  dup2(fd, STDIN_FILENO);
  close(fd);
}

void doIORedirection(struct Tokens input) {
  int outputSignIndex = findOutputRedirection(input);
  int inputSignIndex = findInputRedirection(input);
  if (outputSignIndex >= 0) {
    redirectOutputToFile(input.commands[outputSignIndex + 1]); // the token after the > sign is the name of the file
    input.commands[outputSignIndex] = NULL; // set > to NULL so that exec will exclude output redirection
  }
  if (inputSignIndex >= 0) {
    redirectInputToFile(input.commands[inputSignIndex + 1]); // the token after the < sign is the name of the file
    input.commands[inputSignIndex] = NULL; // set < to NULL so that exec will exclude input redirection
  }
}

void executeCommand(struct Tokens input) {
  fflush(0); // reset IO channels to default values (stdin, stdout and stderr)
  int pid = fork();
  if (pid < 0) {
    printf("Error creating forked process. Error code %d\n", pid);
    exit(1);
  }
  else if (pid == 0) {
    doIORedirection(input);
    execvp(input.commands[0], input.commands);
    perror("execvp");
    _exit(1);
  }
  int *status;
  wait(status);
}

void main(int argc, char **argv) {
  const char delimiter[] = " \t";
  char input[256];
  while (1) {
    printf("> ");
    fgets(input, sizeof(input), stdin);
    trimwhitespace(input);
    struct Tokens commands = split(input, delimiter);
    executeCommand(commands);
  }
}
