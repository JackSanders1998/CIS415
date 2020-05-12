#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Error! Usage: %s <file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  setbuf(stdout, NULL);
  FILE *input;
  FILE *output;
  int numprograms = 0;
  char* lines[50];

  input = freopen(argv[1], "r", stdin);

  if (input == NULL) {
    fprintf(stderr, "unable to open input file\n");
    exit(EXIT_FAILURE);
  }

  else {
    char line[50];
    while (fgets(line, sizeof line, input) != NULL) {

        if ((line)[strlen(line) - 1] == '\n') {
          (line)[strlen(line) - 1] = '\0';
        }

        lines[numprograms] = strdup(line);
        numprograms++;
    }
    output = freopen("part1_out.txt", "w", stdout);
  }

  //---------------------------------------------//

  pid_t pid[5];	// wtf
  char* program;
  char* arguments[50];
  char* saveptr;

  for (int i = 0; i < numprograms; i++) {

    char* token = strtok_r(lines[i], " ", &saveptr);
    int j = 0;

    while (token) {
      arguments[j] = strdup(token);
      token = strtok_r(NULL, " ", &saveptr);
      j++;
    }
    pid[i] = fork();
    if (pid[i] < 0) {
      exit(EXIT_FAILURE);
    }
    if (pid[i] == 0) {

      execvp(arguments[0], arguments);
      fclose(input);
  	fclose(output);
      exit(-1);
      return 0;
    }
    memset(arguments, 0, sizeof(arguments));
  }

  for (int i = 0; i < numprograms; i++) {
    wait(&pid[i]);
  } 

  fclose(input);
  fclose(output);
  return 1;
}




