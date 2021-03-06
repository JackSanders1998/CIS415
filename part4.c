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
    kill(pids[i], SIGSTOP);
    }
}

int child_alive(pid_t *processes, int numprograms) {
	  int status;
    int index = 0;
    pid_t curr_child;

    for (int i = 0; i < numprograms; i++) {
        curr_child = waitpid(processes[i], &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (!WIFEXITED(status)) {
            index++;
          }
    }
    return index;
}

void top_func(int pid) {
  printf("\n----------------------------\n");
  char name[64];
  sprintf(name, "/proc/%d/stat", pid);
  printf("pid = %d\n", pid);
  FILE *file = fopen(name, "r");

  int unused;
  char comm[64];
  char state;
  int ppid;
  long int cstime;
  long int num_threads;
  fscanf(file, "%d %s %c %d %ld", &unused, comm, &state, &ppid, &cstime, &num_threads);
  printf("comm: %s\n", comm);
  printf("state: %c\n", state);
  printf("ppid: %d\n", ppid);
  printf("cstime: %ld\n", cstime);
  printf("num_threads: %ld\n", num_threads);

  fclose(file);

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
        printf("\n\nget pid: %d\n\n", getpid());
        top_func(getpid());
        if (pid[i] < 0) {
          fprintf(stderr, "fork error\n");
          exit(-1);
        }

        if (pid[i] == 0) {
          int sig_flag = SIGUSR1;
          int exec_test = sigwait(&sig_set, &sig_flag);
          if (exec_test == 0) {
            printf("%d\n", getpid());
            int exec;
            exec = execvp(arguments[0], arguments);

            if (exec == -1) {
              exit(-1);
            }

          }
          free(*arguments);
          fclose(input);
          exit(-1);
        }
        free(*arguments);
    }


    sleep(1);
    signaler(pid, SIGUSR1, numprograms);
    signaler(pid, SIGSTOP, numprograms);

    sigset_t schedule;
    sigemptyset(&schedule);
    sigaddset(&schedule, SIGALRM);
    sigprocmask(SIG_BLOCK, &schedule, NULL);
    int status;
    int pid_index = 0;
    int children = numprograms;

    while (child_alive(pid, numprograms) >  0) {
      if (pid[pid_index] != -1) {
        alarm(2);
        kill(pid[pid_index], SIGCONT);
        int sig_flag = SIGALRM;
        sigaddset(&schedule, SIGALRM);
        sigwait(&schedule, &sig_flag);
        kill(pid[pid_index], SIGSTOP);
        kill(pid[pid_index + 1], SIGCONT);
        waitpid(pid[pid_index], &status, WNOHANG | WUNTRACED | WCONTINUED);
        top_func(getpid());
        if (!WIFEXITED(status)) {
          pid[pid_index] == -1;
        }
      }
      if (pid_index == numprograms) {
        pid_index = 0;
      }
      pid_index++;
      children = children - 1;
    }
    waitpid(pid[pid_index], &status, WNOHANG | WUNTRACED | WCONTINUED);



    fclose(input);
    exit(0);
    return 1;
}
