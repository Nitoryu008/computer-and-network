#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

typedef struct
{
  const char *ip;
  int pid;
  int last_status;
} Target;

Target targets[] = {
    {"192.168.0.1", 0, -1},
    {"192.168.1.25", 0, -1},
    {"192.168.1.99", 0, -1},
    {"8.8.8.8", 0, -1},
    {NULL, 0, -1}};

void run_ping(const char *ip)
{
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);

  execlp("ping", "ping", "-c", "1", "-W", "1", ip, NULL);

  perror("execlp");
  exit(127);
}

void reap_children()
{
  int status;
  int pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    for (int i = 0; targets[i].ip != NULL; i++)
    {
      if (targets[i].pid == pid)
      {
        targets[i].pid = 0;

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
          targets[i].last_status = 0;
        }
        else
        {
          targets[i].last_status = 1;
        }
        break;
      }
    }
  }
}

void print_status()
{
  printf("\033[H\033[J");
  printf("--- LAN Device Status ---\n");
  printf("Press Ctrl+C to quit.\n\n");

  for (int i = 0; targets[i].ip != NULL; i++)
  {
    printf("%-15s: ", targets[i].ip);

    if (targets[i].pid != 0)
    {
      printf("Checking...\n");
    }
    else
    {
      switch (targets[i].last_status)
      {
      case 0:
        printf("UP\n");
        break;
      case 1:
        printf("DOWN\n");
        break;
      default:
        printf("Unknown\n");
        break;
      }
    }
  }
  fflush(stdout);
}

int main()
{
  while (1)
  {
    reap_children();

    print_status();

    for (int i = 0; targets[i].ip != NULL; i++)
    {
      if (targets[i].pid == 0)
      {

        int pid = fork();

        if (pid < 0)
        {
          perror("fork");
        }
        else if (pid == 0)
        {
          run_ping(targets[i].ip);
        }
        else
        {
          targets[i].pid = pid;
        }
      }
    }

    sleep(1);
  }

  return 0;
}