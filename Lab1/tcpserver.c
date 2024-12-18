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

#define PORT "58001"

int fd, errcode, fd_tcp;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];
char host[NI_MAXHOST], service[NI_MAXSERV]; // consts in <netdb.h>

int main(void)
{
  fd = socket(AF_INET, SOCK_STREAM, 0); // UDP socket
  if (fd == -1)                         /*error*/
    exit(1);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_STREAM; // UDP socket
  hints.ai_flags = AI_PASSIVE;

  errcode = getaddrinfo(NULL, PORT, &hints, &res);
  if (errcode != 0) /*error*/
    exit(1);

  struct sigaction act;
  memset(&act, 0, sizeof act);
  act.sa_handler = SIG_IGN;
  if (sigaction(SIGPIPE, &act, NULL) == -1) /*error*/
    exit(1);

  n = bind(fd, res->ai_addr, res->ai_addrlen);
  if (n == -1) /*error*/
    exit(1);

  if (listen(fd, 5) == -1) /*error*/
    exit(1);

  while (1)
  {
    addrlen = sizeof(addr);

    if ((fd_tcp = accept(fd, (struct sockaddr *)&addr, &addrlen)) == -1) /*error*/
      exit(1);

    n = read(fd_tcp, buffer, 128);
    if (n == -1) /*error*/
      exit(1);
    write(1, "received: ", 10);
    write(1, buffer, n);

    n = write(fd_tcp, buffer, n);
    if (n == -1) /*error*/
      exit(1);

    close(fd_tcp);

    if ((errcode = getnameinfo((struct sockaddr *)&addr, addrlen, host, sizeof host, service, sizeof service, 0)) != 0)
      fprintf(stderr, "error: getnameinfo: %s\n", gai_strerror(errcode));
    else
      printf("sent by [%s:%s]\n", host, service);
  }

  freeaddrinfo(res);
  close(fd);

  return 0;
}