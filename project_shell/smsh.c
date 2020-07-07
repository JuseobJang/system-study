#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parsing.c"
#include "division.c"

#define BUFSIZE 100

char history[BUFSIZE][BUFSIZE];
int noclobber = 0;
FILE *hist;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char buffer[BUFSIZE];
    char command[BUFSIZE][BUFSIZE];
    hist = fopen("history.txt", "a+");

    char *one;

    int n, i, j, k;
    int last;
    int start;

    int background;
    int brac;
    int con_brac;

    while (1)
    {
        char curdir[BUFSIZE] = {};
        getcwd(curdir, BUFSIZE);
        bzero(buffer, BUFSIZE);
        for (i = 0; i < BUFSIZE; i++)
        {
            memset(command[i], '\0', sizeof(char) * BUFSIZE);
        }

        fprintf(stdout, "%s$ ", curdir);
        fflush(stdout);

        background = 0;
        con_brac = 0;

        if ((n = read(STDIN_FILENO, buffer, BUFSIZE)) < 0) // STDIN_FILENO에 명령어를 받아옴
            error("READ ERROR");

        if (!strcmp(buffer, "\n"))
            continue;
        else if (!strcmp(buffer, "exit\n"))
        {
            fprintf(hist, "%s", buffer);
            fflush(hist);
            fclose(hist);
            exit(0);
            return 0;
        }
        else
        {
            fprintf(hist, "%s", buffer);
            fflush(hist);
            i = parsing(buffer, command);
        }
        last = i + 1;

        if (command[0][0] == '\0')
        {
            i = 1;
        }
        else
        {
            i = 0;
        }
        for (int b = 0; b < last; b++)
        {
            if (!strcmp(command[b], "("))
            {
                con_brac++;
            }
            else if (!strcmp(command[b], ")"))
            {
                con_brac--;
            }
            if (con_brac < 0)
            {
                perror("syntax error");
                goto bye;
            }
        }
        if (con_brac > 0)
        {
            perror("syntax error");
            goto bye;
        }

        if (!strcmp(command[last - 1], "&"))
        {
            background = 1;
            command[--last][0] = '\0';
        }
        division(command, i, last, background, 0);
    bye:;
    }
    fclose(hist);
    return 0;
}
