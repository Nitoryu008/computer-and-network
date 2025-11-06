#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
  int i, pid;

  for (i = 0; i < 3; i++)
  {
    pid = fork();
    if (pid > 0)
    {
      wait((int *)0);
    }

    if (pid == 0 && i == 0)
    {
      execlp("pwd", "pwd", NULL);
      perror("execl failed");
    }

    if (pid == 0 && i == 1)
    {
      execlp("date", "date", NULL);
      perror("execl failed");
    }

    if (pid == 0 && i == 2)
    {
      execlp("whoami", "whoami", NULL);
      perror("execl failed");
    }
  }
}