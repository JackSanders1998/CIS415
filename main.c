#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void signaler(pid_t *children, int siglabel) {
  for (int i = 0; i < 5; i++) {
    kill(children[i], siglabel);
  }
}

int child_alive(pid_t *process) {
    int status;
    int index;
    pid_t curr_child;
    for (int i = 0; i < 5; i++) {
        //curr_child = ;
        if (waitpid(process[i], &status, WNOHANG) == 0)
            index++;
    }
    return index;
}

int main(int argc, char **argv) {
    pid_t processes[5];
    pid_t curproc;
s
    sigset_t signal_test;
    sigemptyset(&signaler);
    int check = sigaddset(&signaler, SIGUSR1);

    sigprocmask(SIG_BLOCK, &signaler, NULL);

        for (int i = 0; i < 5; i++) {
        processes[i] = fork();
        if (processes[i] == 0) {
            printf("Child Process: %i - Waiting for SIGUSR1...\n", getpid());
            int signum = SIGUSR1;
            int result = sigwait(&signal_test, &signum);
            if (result == 0) {
                printf("Signal received\n");
                execvp("./iobound", NULL);
            }
            printf("Child Process: %i sigwait could not execute\n", getpid());
            //exit(EXIT_FAILURE);
        }
        else {
            printf("Parent Process: %i - Waiting to SIGSUR1...\n", getpid());
        }
    }

    printf("Parent Process: %i entering control \n", getpid());
    sleep(3);
    signaler(processes, SIGUSR1);
    signaler(processes, SIGSTOP);
    //pid_t curr_child, next_child;
    //int status_curr, status_next;
    //int alive
    //processes[5] = -1;
    int i = 0;
    while (child_alive(processes) > 1) {
        kill(processes[i%5], SIGSTOP);
        kill(processes[(i+1)%5], SIGCONT);
            //curr_child = waitpid(processes[i], &status_curr, WNOHANG);
            //next_child = waitpid(processes[i+1], &status_next, WNOHANG);
            //printf("curr_child: %d\n", curr_child);
            //printf("next_child: %d\n", next_child);

            /*
            if (waitpid(processes[i], &status_curr, WNOHANG) >= 0) {
                kill(processes[i], SIGSTOP);
            }
            if (waitpid(processes[i+1], &status_curr, WNOHANG) >= 0) {
                kill(processes[i+1], SIGSTOP);
            }
            //i++;
            */

            /*
            if (kill(processes[i], SIGSTOP) > 0) {
                printf("current(%d) child alive\n", i);
            }
            else {
                // do something
                printf("current(%d) child dead\n", i);
            }
            if (kill(processes[i+1], SIGCONT) > 0) {
                printf("next(%d) child alive\n", (i+1));
            }
            else {
                // do something
                printf("next(%d) child dead\n", (i+1));
            }
            */
        i++;
        sleep(1);
    }
    return 0;
}

