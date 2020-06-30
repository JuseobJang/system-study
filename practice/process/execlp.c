#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#define MAXARGS 31
#define MAXFILENAME 1024
#define MAXPATHSTR 2048

extern char **environ;
int myexeclp(const char *file, const char *args, ...)
{
  char *vector[MAXARGS];
  char *path_list, *ptr;
  char *path[MAXPATHSTR];
  int i, j;

  va_list ap;
  va_start(ap, args);
  vector[0] = (char *)args;
  for (i = 1; (vector[i] = va_arg(ap, char *)) != NULL; i++)
    va_end(ap);
  path_list = getenv("PATH");
  ptr = strtok(path_list, ":");

  path[0] = ptr;
  for (i = 1; ptr != NULL; i++)
  {
    ptr = strtok(NULL, ":");
    path[i] = ptr;
  }
  for (j = i - 2; j >= 0; j--)
  {
    sprintf(path[j], "%s/%s", path[j], file);
    execve(path[j], vector, environ);
  }

  return -1;
}

int main(void)
{
  char path[MAXPATHSTR];
  sprintf(path, "PATH=%s:%s", getcwd(NULL, 0), getenv("PATH"));
  putenv(path);
  // prepare the executable file named "hello"
  system("cp hello.tmp hello.c");
  system("clang-7 -pthread -lm -o hello hello.c");
  system("rm hello.c");
  myexeclp("hello", "hello", "-a", "-b", "-c", (char *)0);

  return 0;
}
