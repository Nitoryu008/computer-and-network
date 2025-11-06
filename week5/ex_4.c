#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
  int fd_child_read, fd_parent_read, fd_write;
  int pid;
  char buf[BUFSIZE];
  long n_read;

  if ((fd_write = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
  {
    perror("open (write_file) failed");
    exit(1);
  }

  if ((pid = fork()) < 0)
  {
    perror("fork failed");
    exit(1);
  }
  else if (pid == 0)
  {
    if ((fd_child_read = open(argv[1], O_RDONLY)) < 0)
    {
      perror("open (child_read_file) failed");
      exit(1);
    }

    while ((n_read = read(fd_child_read, buf, BUFSIZE)) > 0)
    {
      if (write(fd_write, buf, n_read) != n_read)
      {
        perror("write (child) failed");
        exit(1);
      }
    }
    if (n_read < 0)
    {
      perror("read (child) failed");
      exit(1);
    }

    close(fd_child_read);
    close(fd_write);
    exit(0);
  }
  else
  {
    wait(NULL);

    if ((fd_parent_read = open(argv[2], O_RDONLY)) < 0)
    {
      perror("open (parent_read_file) failed");
      exit(1);
    }

    while ((n_read = read(fd_parent_read, buf, BUFSIZE)) > 0)
    {
      if (write(fd_write, buf, n_read) != n_read)
      {
        perror("write (parent) failed");
        exit(1);
      }
    }
    if (n_read < 0)
    {
      perror("read (parent) failed");
      exit(1);
    }

    close(fd_parent_read);
    close(fd_write);
  }

  return 0;
}