#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

const char terminator[] = "\vEOL\v";

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

void **appendTerminator(char **strings, int numStrings) {
  char tmp[strlen(terminator)];
  strcpy(tmp, terminator);
  strings[numStrings] = tmp;
}

// Splits a string into a list of strings
char **split(const char string[], const char delimit[]) {
  int numWords = count(string, delimit) + 1;
  char **output = createArray(numWords + 1, 10);
  int j = 0;
  int count = 0;
  for (int i = 0; i < strlen(string); i++) {
    // if delimiter, append \0 to finish word, and then start a new word
    if(contains(delimit, string[i])) {
      output[count][j]='\0';
      count++;
      j=0;
    }
    // else just add the char to the current word
    else
    {
      output[count][j]=string[i];
      j++;
    }
  }
  appendTerminator(output, numWords);
  return output;
}

void printWords(char **words) {
  for (int i = 0; !stringsAreEqual(words[i], terminator); i++) { 
    printf(" %s", words[i]);
  }
}

void executeCommand(char **input) {
  int pid = fork();
  if (pid == 0) {
    execlp(input[0], input[0], NULL);
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
    char **words = split(input, delimiter);
    executeCommand(words);
    destroyArray(words);
  }
}
