#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


//int status_array[50];
int numprograms;

void signaler(pid_t *children, int siglabel) {
  for (int i = 0; i < numprograms; i++) {
    kill(children[i], siglabel);
  }
}

int child_alive(pid_t *process) {
	int status;
    int index;
    pid_t curr_child;

    for (int i = 0; i < numprograms; i++) {
        curr_child = waitpid(process[i], &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (!WIFEXITED(status)) {
            index++;
            process[i] = -1;
          }
    }
    return index;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Error! Usage: %s <file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  setbuf(stdout, NULL);
  FILE *input;
  FILE *output;
  numprograms = 0;
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
    //output = freopen("part3_out.txt", "w", stdout);
  }

  //---------------------------------------------//

  char* program;
  char* arguments[50];
  char* saveptr;

  pid_t pid[50];
  pid_t curr_pid;
  sigset_t signals;
  sigemptyset(&signals);
  int check = sigaddset(&signals, SIGUSR1);
  sigprocmask(SIG_BLOCK, &signals, NULL);

  for (int i = 0; i < numprograms; i++) {

    char* token = strtok_r(lines[i], " ", &saveptr);
    int j = 0;

    while (token) {
      arguments[j] = strdup(token);
      token = strtok_r(NULL, " ", &saveptr);
      j++;
    }

    pid[i] = fork();
    if (pid[i] == 0) {
      printf("Child Process: %i - Waiting for SIGUSR1...\n", getpid());
      int signum = SIGUSR1;
      int result = sigwait(&signals, &signum);
      if (result == 0) {
        printf("Signal received\n");
        execvp(arguments[0], arguments);
      }
      exit(EXIT_FAILURE);
    }

    memset(arguments, 0, sizeof(arguments));
  }

  sleep(1);
  signaler(pid, SIGUSR1);

  int j = 0;
  printf("%d\n", child_alive(pid));
  while (child_alive(pid) > 1) {
    kill(pid[j % numprograms], SIGSTOP);
    kill(pid[(j+1) % numprograms], SIGCONT);
    j++;
  }

  fclose(input);
  fclose(output);
  return 1;
}
