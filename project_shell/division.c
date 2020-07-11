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
        else if (!strcmp(command[i], "("))
        {
            brac = 1;
        }
        else if (!strcmp(command[i], ")"))
        {

            if (strcmp(command[i + 1], ">"))
            {
                brac = 0;
                for (int c = 0; c < b + 1; c++)
                {
                    if (fork() == 0)
                    {
                        execvp(brac_cmd[c][0], brac_cmd[c]);
                    }
                    if (background == 0)
                        wait(NULL);
                }
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
        else if (!strcmp(command[i], "cd"))

        {
            if (!strcmp(command[i + 1], ""))
                chdir(getenv("HOME"));
            else if (chdir(command[++i]) < 0)
            {
                perror("Change Directory Error");
            }
        }
        else if (!strcmp(command[i], "cat") && !strcmp(command[i + 1], ">"))
        {
            char buf[BUFSIZE];
            if (fork() == 0)
            {
                int fd = open(command[i + 2], O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0644);
                if (fd < 0)
                {
                    error("open error");
                }

                dup2(fd, 1);

                while (1)
                {
                    if (scanf("%s", buf) == EOF)

                    {
                        exit(0);
                    }

                    else
                    {
                        printf("%s\n", buf);
                    }
                }
            }
            if (background == 0)
            {
                wait(0);
            }
            i = i + 2;
        }

        else if (!strcmp(command[i], ">"))
        {
            if (!strcmp(command[i - 1], ")"))
            {
                if (!strcmp(command[i + 1], ">"))
                {
                    fd = open(command[i + 2], O_CREAT | O_WRONLY | O_APPEND, 0644);

                    dup2(fd, 1);
                    for (int c = 0; c < b + 1; c++)
                    {
                        if (fork() == 0)
                        {
                            execvp(brac_cmd[c][0], brac_cmd[c]);
                        }
                        if (background == 0)
                            wait(NULL);
                    }
                    i = i + 2;
                    dup2(save_stdout, 1);
                    fd = 0;
                }
                else if (!strcmp(command[i + 1], "|"))
                {
                    fd = open(command[i + 2], O_CREAT | O_WRONLY | O_TRUNC, 0644);

                    dup2(fd, 1);
                    for (int c = 0; c < b + 1; c++)
                    {
                        if (fork() == 0)
                        {
                            execvp(brac_cmd[c][0], brac_cmd[c]);
                        }
                        if (background == 0)
                            wait(NULL);
                    }
                    i = i + 2;
                    dup2(save_stdout, 1);
                    fd = 0;
                }
                else
                {
                    if (noclobber == 0)
                    {
                        fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    }
                    else
                    {
                        fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC | O_EXCL, 0644);
                    }

                    dup2(fd, 1);
                    for (int c = 0; c < b + 1; c++)
                    {
                        if (fork() == 0)
                        {
                            execvp(brac_cmd[c][0], brac_cmd[c]);
                        }
                        if (background == 0)
                            wait(NULL);
                    }
                    i = i + 1;
                    dup2(save_stdout, 1);
                    fd = 0;
                }
            }
            else if (strcmp(command[i - 1], ")") && brac == 1)
            {
                if (!strcmp(command[i + 1], ">"))
                {
                    fd = open(command[i + 2], O_CREAT | O_WRONLY | O_APPEND, 0644);

                    dup2(fd, 1);

                    if (fork() == 0)
                    {
                        execvp(brac_cmd[b][0], brac_cmd[b]);
                    }
                    if (background == 0)
                        wait(NULL);
                    b--;
                    i = i + 2;
                    dup2(save_stdout, 1);
                    fd = 0;
                }
                else if (!strcmp(command[i + 1], "|"))
                {
                    fd = open(command[i + 2], O_CREAT | O_WRONLY | O_TRUNC, 0644);

                    dup2(fd, 1);

                    if (fork() == 0)
                    {
                        execvp(brac_cmd[b][0], brac_cmd[b]);
                    }
                    if (background == 0)
                        wait(NULL);
                    b--;
                    i = i + 2;
                    dup2(save_stdout, 1);
                    fd = 0;
                }

                else
                {
                    if (noclobber == 0)
                    {
                        fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                    }
                    else
                    {
                        fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC | O_EXCL, 0644);
                    }

                    dup2(fd, 1);

                    if (fork() == 0)
                    {
                        execvp(brac_cmd[b][0], brac_cmd[b]);
                    }
                    if (background == 0)
                        wait(NULL);
                    b--;
                    i = i + 1;
                    dup2(save_stdout, 1);
                    fd = 0;
                }
            }
            else if (strcmp(command[i - 1], ")"))
            {
                if (!strcmp(command[i + 1], ">"))
                {
                    if (fork() == 0)
                    {
                        dup2(fd, 0);

                        int fd = open(command[i + 2], O_CREAT | O_WRONLY | O_APPEND, 0644);
                        if (fd < 0)
                        {
                            perror("File Open Error");
                            exit(1);
                        }

                        dup2(fd, 1);
                        execvp(seob[j][0], seob[j]);
                        execvp(seob[j - 1][0], seob[j - 1]);
                    }

                    if (background == 0)
                    {
                        wait(NULL);
                    }
                    i = i + 2;
                    j++;
                    k = 0;
                }
                else if (!strcmp(command[i + 1], "|"))
                {
                    if (fork() == 0)
                    {
                        dup2(fd, 0);

                        int fd = open(command[i + 2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        if (fd < 0)
                        {
                            perror("File Open Error");
                            exit(1);
                        }
                        dup2(fd, 1);
                        execvp(seob[j][0], seob[j]);
                        execvp(seob[j - 1][0], seob[j - 1]);
                    }
                    if (background == 0)
                    {
                        wait(NULL);
                    }
                    i = i + 2;
                    j++;
                    k = 0;
                }

                else
                {
                    if (fork() == 0)
                    {
                        dup2(fd, 0);

                        int fd;
                        if (noclobber == 0)
                        {
                            fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        }
                        else
                        {
                            fd = open(command[i + 1], O_CREAT | O_WRONLY | O_TRUNC | O_EXCL, 0644);
                        }

                        if (fd < 0)
                        {
                            perror("File Open Error");
                            exit(1);
                        }
                        dup2(fd, 1);
                        execvp(seob[j][0], seob[j]);
                        execvp(seob[j - 1][0], seob[j - 1]);
                    }
                    if (background == 0)
                    {
                        wait(NULL);
                    }
                    i = i + 1;
                    j++;
                    k = 0;
                }
            }
        }
        else if (!strcmp(command[i], "<"))
        {
            if (fork() == 0)
            {

                int fd = open(command[i + 1], O_RDONLY, 0);
                if (fd < 0)
                {
                    perror("File Open Error");
                    exit(1);
                }
                dup2(fd, 0);
                execvp(seob[j][0], seob[j]);
            }
            if (background == 0)
            {
                wait(NULL);
            }
            i = i + 1;
            j++;
            k = 0;
        }
        else if (!strcmp(command[i], "|"))
        {

            int files[2];
            pipe(files);

            if (fork() == 0)
            {
                dup2(fd, 0);
                dup2(files[1], 1);
                close(files[0]);
                execvp(seob[j][0], seob[j]);
            }
            if (background == 0)
            {
                wait(NULL);
            }
            close(files[1]);

            fd = files[0];

            j++;
            k = 0;
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
