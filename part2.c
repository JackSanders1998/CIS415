#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


void signaler(pid_t *pids, int label, int numprograms) {
    for (int i = 0; i < numprograms; i++) {
    kill(pids[i], label);
    }
}

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

    // copy each line into an array & count number of lines
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

    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sig_set, NULL);

    char* arguments[50];
    char* saveptr;
    int j;

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
          fprintf(stderr, "fork error\n");
          exit(-1);
        }

        if (pid[i] == 0) {
          printf("child process: %d\n", getpid());
          int sig_flag = SIGUSR1;
          int exec_test = sigwait(&sig_set, &sig_flag);
          if (exec_test == 0) {
            int exec = execvp(arguments[0], arguments);
            if (exec == -1) {
              fprintf(stderr, "exec error");
            exit(-1);
            }
          }
          free(*arguments);

          fclose(input);
          exit(-1);
        }

        memset(arguments, 0, sizeof(arguments));
        free(*arguments);
    }


    sleep(1);
    signaler(pid, SIGUSR1, numprograms);
    printf("Sending SIGUSR\n");
    signaler(pid, SIGSTOP, numprograms);
    printf("Sending SIGSTOP\n");
    signaler(pid, SIGCONT, numprograms);
    printf("Sending SIGCONT\n");

    for (int i = 0; i < numprograms; i++) {
      wait(&pid[i]);
    }


    fclose(input);
    exit(0);
    return 1;
}
