#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>



void signaler(pid_t *processes, int label, int numprograms) {
    for (int i = 0; i < numprograms; i++) {
    kill(processes[i], label);
    //kill(processes[i], SIGSTOP);
    }
}

//void alarmHandler(int sig) {
//  signal(SIGALRM, alarmHandler);
//}

int child_alive(pid_t *processes, int numprograms) {
	  int status;
    int index;
    pid_t curr_child;

    for (int i = 0; i < numprograms; i++) {
      //printf("child alive");
        curr_child = waitpid(processes[i], &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (!WIFEXITED(status)) {
            index++;
            processes[i] = -1;
          }
    }
    return index;
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
          printf("forking %d\n", pid[i]);
          if (pid[i] < 0) {
            fprintf(stderr, "fork error\n");
            exit(-1);
          }

          if (pid[i] == 0) {
            printf(" %s %s \n", arguments[0], arguments[1]);
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


    /*
    sleep(1);
    signaler(pid, SIGUSR1, numprograms);
    signaler(pid, SIGSTOP, numprograms);
    signaler(pid, SIGCONT, numprograms);

    for (int i = 0; i < numprograms; i++) {
      wait(&pid[i]);
    }
    */
    sigset_t signal;
    sigemptyset(&signal);
    sigaddset(&signal, SIGALRM);
    sigprocmask(SIG_BLOCK, &signal, NULL);

    int pid_index = 0;
    //int next_index = 1;
    printf("%d\n", child_alive(pid, numprograms));
    while (child_alive(pid, numprograms) > 1) {
      if (pid[pid_index] != -1) {
        alarm(2);
        kill(pid[pid_index], SIGCONT);
        //sigset_t sig_set;
        //int signum = SIGALRM;
        sigset_t signal; int signum = SIGALRM;
        sigaddset(&signal, SIGALRM);
        sigwait(&signal, &signum);


        //kill(pid[pid_index % numprograms], SIGSTOP);
        pid_index++;
      }
    }

    fclose(input);
    exit(0);
    return 1;
}
