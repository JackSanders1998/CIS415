#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void signaler(pid_t *children, int siglabel, int numprograms) {
    for (int i = 0; i < numprograms; i++) {
    kill(children[i], siglabel);
    kill(children[i], SIGSTOP);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error! Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    FILE *input;
    //FILE *output;
    int numprograms = 0;
    char* programs[50];

    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        fprintf(stderr, "unable to open input file\n");
        exit(-1);
    }

    else {

        char line[50];
        while (fgets(line, sizeof line, input) != NULL) {

            if ((line)[strlen(line) - 1] == '\n') {
                (line)[strlen(line) - 1] = '\0';
            }

            programs[numprograms] = strdup(line);
            numprograms++;
        }
        //output = freopen("part1_out.txt", "w", stdout);
    }

  //---------------------------------------------//

    char* program;
    char* arguments[50];
    char* saveptr;

    pid_t pid[50];
    sigset_t signals;
    sigemptyset(&signals);
    int check = sigaddset(&signals, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signals, NULL);

    for (int i = 0; i < numprograms; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {

        	exit(-1);
        }
        else if (pid[i] == 0) {
            //strcpy(line_token, lines[i]);
            char* token = strtok_r(programs[i], " ", &saveptr);
            int j = 0;

            while (token) {
            arguments[j] = strdup(token);
            token = strtok_r(NULL, " ", &saveptr);
            j++;
        }
        arguments[j] == NULL;
        printf("WTF\n");
             for (int b = 0; b < j+1; b++) {
                fprintf(stdout, "here: %s\n", arguments[b]);
              }




          int signum = SIGUSR1;
          int result = sigwait(&signals, &signum);


          if (result == 0) {
            execvp(arguments[0], arguments);
          }
        fclose(input);
        //free(token);
  		fprintf(stderr, "execvp");
  		exit(-1);
    }
    memset(arguments, 0, sizeof(arguments));
  }

  sleep(1);
  signaler(pid, SIGUSR1, numprograms);
  signaler(pid, SIGSTOP, numprograms);
  signaler(pid, SIGCONT, numprograms);

  for (int i = 0; i < numprograms; i++) {
    wait(&pid[i]);
  }

  fclose(input);
  exit(0);
}
