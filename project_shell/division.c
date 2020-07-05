#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFSIZE 100
FILE *hist;

int noclobber = 0;

void division(char command[BUFSIZE][BUFSIZE], int start, int last, int background, int brac)
{
    int i;
    int j;
    int k;
    int b;
    char *seob[BUFSIZE][BUFSIZE];
    char *brac_cmd[BUFSIZE][BUFSIZE];
    int fd = 0;
    int save_stdout = dup(1);

    for (i = start, j = 0, k = 0, b = 0; i < last; i++)
    {

        if (!strcmp(command[i], ";"))
        {
            if (brac == 0)
            {
                if (fork() == 0)
                {
                    dup2(fd, 0);
                    execvp(seob[j][0], seob[j]);
                }
                if (background == 0)
                {
                    wait(NULL);
                }
                j++;
                k = 0;
            }
            else
            {
                if (background == 0)
                {
                    wait(NULL);
                }
                b++;
                k = 0;
            }
        }
        else if (!strcmp(command[i], "!"))
        {
            int num = atoi(command[++i]);
            int new_start;
            int new_last;
            char list;
            int find = 0;
            int c = 0;
            char new_buffer[BUFSIZE];
            char new_command[BUFSIZE][BUFSIZE];
            rewind(hist);
            while ((list = fgetc(hist)) != EOF)
            {
                new_buffer[c++] = list;
                if (list == '\n' && find != num)
                {
                    find++;
                    c = 0;
                }
                if (find > num)
                {
                    break;
                }
            }
            new_buffer[c] = '\0';

            new_last = parsing(new_buffer, new_command) + 1;
            if (new_command[0][0] == '\0')
            {
                new_start = 1;
            }
            else
            {
                new_start = 0;
            }
            division(new_command, new_start, new_last, background, brac);
        }
        else if (!strcmp(command[i], "history"))
        {
            rewind(hist);
            char list;
            while ((list = fgetc(hist)) != EOF)
            {
                printf("%c", list);
            }
        }

        else
        {
            if (brac == 0)
            {
                seob[j][k] = command[i];
                seob[j][++k] = (char *)0;
            }
            else
            {
                brac_cmd[b][k] = command[i];
                brac_cmd[b][++k] = (char *)0;
            }
        }
    }

    if (fork() == 0)
    {
        dup2(fd, 0);
        execvp(seob[j][0], seob[j]);
    }
    if (background == 0)
    {
        wait(NULL);
    }
}
