#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void signaler(pid_t *processes, int label, int numprograms) {
    for (int i = 0; i < numprograms; i++) {
    kill(processes[i], label);
    //kill(processes[i], SIGSTOP);
    }
}

void top_func(int pid) {
  //system("clear");
  //system("clear");
  char name[50];
  sprintf(name, "/proc/%d/stat", pid);
  printf("pid = %d\n", pid);
  FILE *file = fopen(name, "r");

  int unused;
  char comm[1000];
  char state;
  int ppid;
  fscanf(file, "%d %s %c %d", &unused, comm, &state, &ppid);
  printf("command = %s\n", comm);
  printf("state = %c\n", state);
  printf("parent pid = %d\n", ppid);
  fclose(file);

}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error! Usage: %s <file>\n", argv[0]);
        exit(-1);
    }

    FILE *input;
    int numprograms = 0;
    char* programs[50];
    char line[50];

    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        fprintf(stderr, "unable to open input file\n");
        exit(-1);
    }

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
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signal, NULL);

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
          int sig_flag = SIGUSR1;
          int exec_test = sigwait(&signal, &sig_flag);
          if (exec_test == 0) {
            execvp(arguments[0], arguments);
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
    signaler(pid, SIGSTOP, numprograms);
    signaler(pid, SIGCONT, numprograms);


    for (int i = 0; i < numprograms; i++) {
      int pid_num = getpid();
      top_func(pid_num);
      wait(&pid[i]);
    }

    fclose(input);
    exit(0);
    return 1;
}
