#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    // usage check
    if (argc != 2) {
        fprintf(stderr, "Error! Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    FILE *input;
    int numprograms = 0;
    char* programs[50];
    char line[50];

    // error check
    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        fprintf(stderr, "unable to open input file\n");
        exit(-1);
    }

    // copy each line of file into array & count number of lines
    else {
        while (fgets(line, sizeof line, input) != NULL) {

            if ((line)[strlen(line) - 1] == '\n') {
                (line)[strlen(line) - 1] = '\0';
            }
            programs[numprograms] = strdup(line);
            numprograms++;
        }
      }

  //---------------------------------------------//

    pid_t pid[numprograms];
    char* arguments[50];
    char* saveptr;
    int j;


    // loop throuch each line in the file
    for (int i = 0; i < numprograms; i++) {
        char* token = strtok_r(programs[i], " ", &saveptr);
        j = 0;
        while (token) {
            arguments[j] = strdup(token);
            token = strtok_r(NULL, " ", &saveptr);
            j++;
        }
        arguments[j] = NULL;

        pid[i] = fork();
        if (pid[i] < 0) {
          exit(-1);
        }
        if (pid[i] == 0) {
            execvp(arguments[0], arguments);
            fclose(input);
            exit(-1);
        }
    }

    for (int i = 0; i < numprograms; i++) {
        wait(&pid[i]);
    }

    fclose(input);
    exit(0);
    return 1;
}
