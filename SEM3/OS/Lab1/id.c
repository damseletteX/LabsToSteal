#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void start(int num)
{
    printf("Процесс №%d: PID = %d, PPID = %d\n", num, getpid(), getppid());
    fflush(stdout);
}

void birth(int parent_pid, int child_pid)
{
    printf("Процесс с ID %d породил процесс с ID %d\n", parent_pid, child_pid);
    fflush(stdout);
}

void end(int num)
{
    printf("Процесс №%d: PID = %d, PPID = %d завершает работу\n", num, getpid(), getppid());
    fflush(stdout);
    exit(0);
}

int main(void)
{
    start(1);

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid2 == 0)
    {
        start(2);
        end(2);
    }
    birth(getpid(), pid2);

    pid_t pid3 = fork();
    if (pid3 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid3 == 0)
    {
        start(3);

        pid_t pid5 = fork();
        if (pid5 < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid5 == 0)
        {
            start(5);

            pid_t pid7 = fork();
            if (pid7 < 0)
            {
                perror("fork");
                exit(1);
            }
            if (pid7 == 0)
            {
                start(7);
                end(7);
            }
            birth(getpid(), pid7);
            wait(NULL);
            end(5);
        }
        birth(getpid(), pid5);

        pid_t pid6 = fork();
        if (pid6 < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid6 == 0)
        {
            start(6);
            end(6);
        }
        birth(getpid(), pid6);

        wait(NULL);
        wait(NULL);
        end(3);
    }
    birth(getpid(), pid3);

    pid_t pid4 = fork();
    if (pid4 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid4 == 0)
    {
        start(4);
        end(4);
    }
    birth(getpid(), pid4);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("Процесс №1: PID = %d запускает вместо себя программу ls\n", getpid());
    fflush(stdout);

    char *args[] = {"ls", "-l", NULL};
    execvp(args[0], args);
    perror("execvp");
    return 1;
}