#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#define PORT "58001"


int main(void)
{
  int fd, errcode, fd_tcp, ret;
  ssize_t n;
  socklen_t addrlen;
  struct addrinfo hints, *res;
  struct sockaddr_in addr;
  char buffer[128];
  char host[NI_MAXHOST], service[NI_MAXSERV]; // consts in <netdb.h>
  pid_t pid;

  fd = socket(AF_INET, SOCK_STREAM, 0); // UDP socket
  if (fd == -1)                         /*error*/
  {
    printf("Socket failed\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // UDP socket
  hints.ai_flags = AI_PASSIVE;

  errcode = getaddrinfo(NULL, PORT, &hints, &res);
  if (errcode != 0) /*error*/
  {
    printf("getaddrinfo failed\n");
    exit(1);
  }

  struct sigaction act;
  memset(&act, 0, sizeof act);
  act.sa_handler = SIG_IGN;
  if (sigaction(SIGPIPE, &act, NULL) == -1) /*error*/
  {
    printf("Sigaction failed\n");
    exit(1);
  }

  n = bind(fd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /*error*/
  {
    printf("Bind failed\n");
    exit(1);
  }

  if (listen(fd, 5) == -1) /*error*/
  {
    printf("Listen failed\n");
    exit(1);
  }

  while (1)
  {
    addrlen = sizeof(addr);

    do
      fd_tcp = accept(fd, (struct sockaddr *)&addr, &addrlen);
    while (fd_tcp == -1 && errno == EINTR);

    if (fd_tcp == -1)
    {
      printf("Accept failed\n");
      exit(1);
    }

    if ((pid = fork()) == -1) /*error*/
    {
      printf("Fork failed\n");
      exit(1);
    }
    else if (pid == 0) // child process
    {
      close(fd);

      n = read(fd_tcp, buffer, 128);
      if (n == -1) /*error*/
        exit(1);
      write(1, "received: ", 10);
      write(1, buffer, n);

      n = write(fd_tcp, buffer, n);
      if (n == -1) /*error*/
        exit(1);

      close(fd_tcp);
      exit(0);
    }
    // parent process
    do
      ret = close(fd_tcp);
    while (ret == -1 && errno == EINTR);

    if ((errcode = getnameinfo((struct sockaddr *)&addr, addrlen, host, sizeof host, service, sizeof service, 0)) != 0)
      fprintf(stderr, "error: getnameinfo: %s\n", gai_strerror(errcode));
    else
      printf("sent by [%s:%s]\n", host, service);
  }

  freeaddrinfo(res);

  return 0;
}