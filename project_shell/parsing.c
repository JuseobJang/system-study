#define BUFSIZE 100

int parsing(char *buffer, char command[BUFSIZE][BUFSIZE])
{
    int i;
    int j;
    char *one;
    for (i = 0; i < BUFSIZE; i++)
    {
        memset(command[i], '\0', sizeof(char) * BUFSIZE);
    }
    for (one = buffer, i = 0, j = 0; *one != '\n'; one++)
    {
        if (*one == ' ')
        {
        }
        else if (((*(one) == ';') || (*(one) == '(') || (*(one) == ')') || (*(one) == '>') || (*(one) == '|') || (*(one) == '&') || (*(one) == '<') || (*(one) == '!')))
        {
            j = 0;

            command[++i][j] = *one;
            command[i][j + 1] = '\0';
        }
        else
        {
            if ((*(one - 1) == ' ') || (*(one - 1) == ';') || (*(one - 1) == '(') || (*(one - 1) == ')') || (*(one - 1) == '>') || (*(one - 1) == '|') || (*(one - 1) == '&') || (*(one - 1) == '<') || (*(one - 1) == '!'))
            {
                i++;
                j = 0;
            }

            command[i][j++] = *one;
            command[i][j] = '\0';
        }
    }
    return i;
}