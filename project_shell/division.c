#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFSIZE 100

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
