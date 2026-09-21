#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void report(int num)
{
    printf("Процесс №%d: PID = %d, PPID = %d\n", num, getpid(), getppid());
    fflush(stdout);
}

void spawn(int parent_pid, int child_pid)
{
    printf("Процесс с ID %d породил процесс с ID %d\n", parent_pid, child_pid);
    fflush(stdout);
}

void finish(int num)
{
    printf("Процесс №%d: PID = %d, PPID = %d завершает работу\n", num, getpid(), getppid());
    fflush(stdout);
    exit(0);
}

int main(void)
{
    report(1);

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid2 == 0)
    {
        report(2);
        finish(2);
    }
    spawn(getpid(), pid2);

    pid_t pid3 = fork();
    if (pid3 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid3 == 0)
    {
        report(3);

        pid_t pid5 = fork();
        if (pid5 < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid5 == 0)
        {
            report(5);

            pid_t pid7 = fork();
            if (pid7 < 0)
            {
                perror("fork");
                exit(1);
            }
            if (pid7 == 0)
            {
                report(7);
                finish(7);
            }
            spawn(getpid(), pid7);
            wait(NULL);
            finish(5);
        }
        spawn(getpid(), pid5);

        pid_t pid6 = fork();
        if (pid6 < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid6 == 0)
        {
            report(6);
            finish(6);
        }
        spawn(getpid(), pid6);

        wait(NULL);
        wait(NULL);
        finish(3);
    }
    spawn(getpid(), pid3);

    pid_t pid4 = fork();
    if (pid4 < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid4 == 0)
    {
        report(4);
        finish(4);
    }
    spawn(getpid(), pid4);

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