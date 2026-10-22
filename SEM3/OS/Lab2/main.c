#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

pid_t *child_pids;
int child_count = 0;

void signalHandler(int signum) {
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGTERM);
        }
    }
    exit(0);
}

typedef struct {
    pid_t pid;
    time_t start_time;
} Process;

void createProcess(char **argv_args, Process *p) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv_args[0], argv_args);
        perror("execvp failed");
        exit(1);
    } else if (pid == -1) {
        p->pid = -1;
    } else {
        p->pid = pid;
        p->start_time = time(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s number seconds process\n", argv[0]);
        return 1;
    }

    int number = atoi(argv[1]);
    int seconds = atoi(argv[2]);
    char *process_str = argv[3];

    char *argv_args[64];
    int arg_count = 0;
    char *token = strtok(process_str, " ");
    
    while (token != NULL && arg_count < 63) {
        argv_args[arg_count] = token;
        arg_count++;
        token = strtok(NULL, " ");
    }
    argv_args[arg_count] = NULL;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    Process *processes = malloc(number * sizeof(Process));
    child_pids = malloc(number * sizeof(pid_t));
    child_count = number;

    for (int i = 0; i < number; i++) {
        createProcess(argv_args, &processes[i]);
        if (processes[i].pid == -1) {
            return 1;
        }
        child_pids[i] = processes[i].pid;
    }

    while (1) {
        for (int i = 0; i < number; i++) {
            Process *p = &processes[i];
            int status;
            pid_t res = waitpid(p->pid, &status, WNOHANG);

            if (res > 0) {
                printf("The process with PID %d has been terminated by itself.\n", p->pid);
                createProcess(argv_args, p);
                if (p->pid == -1) return 1;
                child_pids[i] = p->pid;
            } else if (res == 0) {
                if (time(NULL) - p->start_time > seconds) {
                    kill(p->pid, SIGTERM);
                    printf("The process with PID %d has been terminated.\n", p->pid);
                    waitpid(p->pid, &status, 0);
                    
                    createProcess(argv_args, p);
                    if (p->pid == -1) return 1;
                    child_pids[i] = p->pid;
                }
            }
        }
        sleep(1);
    }

    free(processes);
    free(child_pids);
    return 0;
}
